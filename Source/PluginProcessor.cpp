#include "PluginProcessor.h"
#include "PluginEditor.h"

X1LEAudioProcessor::X1LEAudioProcessor()
: AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                  .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
  parameters(*this, nullptr, "PARAMETERS", createParameters()) {}

juce::AudioProcessorValueTreeState::ParameterLayout X1LEAudioProcessor::createParameters() {
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    auto add = [&p](const char* id, const char* name, float min, float max, float def) {
        p.add(std::make_unique<juce::AudioParameterFloat>(id, name,
            juce::NormalisableRange<float>(min, max, 0.01f), def));
    };
    add("input","Input",-18.0f,18.0f,0.0f);
    add("gate","Gate",0.0f,1.0f,0.08f);
    add("lowcut","Low Cut",60.0f,180.0f,85.0f);
    add("flanger","Flanger",0.0f,1.0f,0.0f);
    add("delay","Delay",0.0f,1.0f,0.0f);
    add("reverb","Reverb",0.0f,1.0f,0.0f);
    add("width","Stereo Width",0.0f,1.0f,0.5f);
    add("presence","Presence",0.0f,12.0f,2.0f);
    add("air","Air",0.0f,12.0f,1.5f);
    add("comp","Comp",0.0f,1.0f,0.30f);
    add("deess","De-ess",0.0f,1.0f,0.22f);
    add("drive","Drive",0.0f,1.0f,0.04f);
    add("mix","Mix",0.0f,1.0f,1.0f);
    add("output","Output",-18.0f,6.0f,0.0f);
    return p;
}

void X1LEAudioProcessor::prepareToPlay(double sr, int) {
    juce::dsp::ProcessSpec spec { sr, 512, static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    for (int c = 0; c < 2; ++c) {
        highPass[c].prepare(spec); presence[c].prepare(spec); airShelf[c].prepare(spec);
        essHighPass[c].prepare(spec); essLowPass[c].prepare(spec);
        highPass[c].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 75.0);
        presence[c].coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, 3200.0, 0.85f, 1.65f);
        airShelf[c].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sr, 10500.0, 0.7f, 1.8f);
        essHighPass[c].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 5600.0);
        essLowPass[c].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, 10500.0);
        compEnvelope[c] = essEnvelope[c] = 0.0f;
    }
    gateEnvelope = 0.0f;
    gateGain = { 1.0f, 1.0f };
    flangeBuffer[0].assign(4096, 0.0f); flangeBuffer[1].assign(4096, 0.0f);
    const auto delaySamples = static_cast<size_t>(sr * 0.7);
    delayBuffer[0].assign(delaySamples, 0.0f); delayBuffer[1].assign(delaySamples, 0.0f);
    flangeWrite = { 0, 0 };
    delayWrite = { 0, 0 };
    flangePhase = { 0.0, 0.0 };
    currentLowCut = 85.0f;
    reverb.prepare(spec);
    reverb.reset();
}

void X1LEAudioProcessor::releaseResources() {}

void X1LEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(2, buffer.getNumChannels());
    const float inputGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("input")->load());
    const float outputGain = juce::Decibels::decibelsToGain(parameters.getRawParameterValue("output")->load());
    const float gate = parameters.getRawParameterValue("gate")->load();
    const float lowCut = parameters.getRawParameterValue("lowcut")->load();
    const float flanger = parameters.getRawParameterValue("flanger")->load();
    const float delay = parameters.getRawParameterValue("delay")->load();
    const float reverbAmount = parameters.getRawParameterValue("reverb")->load();
    const float stereoWidth = parameters.getRawParameterValue("width")->load();
    if (std::abs(lowCut - currentLowCut) > 0.5f) {
        for (int c = 0; c < 2; ++c)
            highPass[c].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), lowCut);
        currentLowCut = lowCut;
    }
    const float comp = parameters.getRawParameterValue("comp")->load();
    const float drive = parameters.getRawParameterValue("drive")->load();
    const float mix = parameters.getRawParameterValue("mix")->load();
    const float presenceAmount = parameters.getRawParameterValue("presence")->load();
    const float airAmount = parameters.getRawParameterValue("air")->load();
    const float deess = parameters.getRawParameterValue("deess")->load();

    const float compAttack = std::exp(-1.0f / (0.012f * static_cast<float>(getSampleRate())));
    const float compRelease = std::exp(-1.0f / (0.115f * static_cast<float>(getSampleRate())));
    const float gateAttack = std::exp(-1.0f / (0.008f * static_cast<float>(getSampleRate())));
    const float gateRelease = std::exp(-1.0f / (0.090f * static_cast<float>(getSampleRate())));
    const float essAttack = std::exp(-1.0f / (0.002f * static_cast<float>(getSampleRate())));
    const float essRelease = std::exp(-1.0f / (0.075f * static_cast<float>(getSampleRate())));

    for (int c = 0; c < channels; ++c) {
        auto* data = buffer.getWritePointer(c);
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            const float dry = data[i];
            float y = highPass[c].processSample(dry * inputGain);
            const float presenceProcessed = presence[c].processSample(y);
            y += (presenceProcessed - y) * (presenceAmount / 12.0f) * 0.70f;
            const float airProcessed = airShelf[c].processSample(y);
            y += (airProcessed - y) * (airAmount / 12.0f) * 0.55f;

            const float level = std::abs(y);
            const float gateCoeff = level > gateEnvelope ? gateAttack : gateRelease;
            gateEnvelope = gateCoeff * gateEnvelope + (1.0f - gateCoeff) * level;
            const float gateThreshold = 0.0012f + gate * 0.022f;
            const float gateTarget = gateEnvelope < gateThreshold ? (1.0f - gate * 0.82f) : 1.0f;

            const float compCoeff = level > compEnvelope[c] ? compAttack : compRelease;
            compEnvelope[c] = compCoeff * compEnvelope[c] + (1.0f - compCoeff) * level;
            const float threshold = 0.32f - comp * 0.17f;
            const float ratio = 1.0f + comp * 3.0f;
            float compressedLevel = compEnvelope[c];
            if (compressedLevel > threshold) {
                const float reduced = threshold + (compressedLevel - threshold) / ratio;
                y *= reduced / juce::jmax(compressedLevel, 0.00001f);
            }

            float essBand = essHighPass[c].processSample(y);
            essBand = essLowPass[c].processSample(essBand);
            const float essLevel = std::abs(essBand);
            const float essCoeff = essLevel > essEnvelope[c] ? essAttack : essRelease;
            essEnvelope[c] = essCoeff * essEnvelope[c] + (1.0f - essCoeff) * essLevel;
            const float essThreshold = 0.018f;
            const float essReduction = deess * juce::jlimit(0.0f, 0.72f,
                (essEnvelope[c] - essThreshold) * 11.0f);
            y -= essBand * essReduction;

            if (drive > 0.001f) {
                const float driven = std::tanh(y * (1.0f + drive * 1.8f));
                y = y * (1.0f - drive * 0.32f) + driven * (drive * 0.32f);
            }

            const float gateGainCoeff = gateTarget < gateGain[c]
                ? std::exp(-1.0f / (0.012f * static_cast<float>(getSampleRate())))
                : std::exp(-1.0f / (0.180f * static_cast<float>(getSampleRate())));
            gateGain[c] = gateGainCoeff * gateGain[c] + (1.0f - gateGainCoeff) * gateTarget;
            y *= gateGain[c];

            const int fSize = static_cast<int>(flangeBuffer[c].size());
            const float lfo = 0.5f + 0.5f * std::sin(static_cast<float>(flangePhase[c]));
            const int fDelay = juce::jlimit(1, fSize - 2, 80 + static_cast<int>(lfo * 300.0f));
            const int fRead = (flangeWrite[c] - fDelay + fSize) % fSize;
            const float fSample = flangeBuffer[c][static_cast<size_t>(fRead)];
            flangeBuffer[c][static_cast<size_t>(flangeWrite[c])] = y;
            y += fSample * flanger * 0.38f;

            const int dSize = static_cast<int>(delayBuffer[c].size());
            const int dDelay = juce::jlimit(1, dSize - 2, static_cast<int>(getSampleRate() * 0.23));
            const int dRead = (delayWrite[c] - dDelay + dSize) % dSize;
            const float dSample = delayBuffer[c][static_cast<size_t>(dRead)];
            delayBuffer[c][static_cast<size_t>(delayWrite[c])] = y + dSample * 0.25f;
            y += dSample * delay * 0.35f;
            const float wet = y;
            float result = (dry * (1.0f - mix) + wet * mix) * outputGain;
            const float magnitude = std::abs(result);
            if (magnitude > 0.97f)
                result = std::copysign(0.97f + 0.03f * std::tanh((magnitude - 0.97f) * 8.0f), result);
            data[i] = result;
            flangeWrite[c] = (flangeWrite[c] + 1) % static_cast<int>(flangeBuffer[c].size());
            delayWrite[c] = (delayWrite[c] + 1) % static_cast<int>(delayBuffer[c].size());
            flangePhase[c] += 0.00022;
            if (flangePhase[c] > juce::MathConstants<double>::twoPi)
                flangePhase[c] -= juce::MathConstants<double>::twoPi;
        }
    }
    if (reverbAmount > 0.001f && buffer.getNumChannels() >= 2) {
        juce::dsp::Reverb::Parameters rp;
        rp.roomSize = 0.58f; rp.damping = 0.48f;
        rp.wetLevel = reverbAmount * 0.30f; rp.dryLevel = 1.0f; rp.width = 1.0f;
        reverb.setParameters(rp);
        juce::dsp::AudioBlock<float> reverbBlock(buffer);
        juce::dsp::ProcessContextReplacing<float> reverbContext(reverbBlock);
        reverb.process(reverbContext);
    }
    if (channels >= 2 && std::abs(stereoWidth - 0.5f) > 0.001f) {
        auto* left = buffer.getWritePointer(0);
        auto* right = buffer.getWritePointer(1);
        const float sideGain = stereoWidth * 2.0f;
        for (int i = 0; i < buffer.getNumSamples(); ++i) {
            const float mid = 0.5f * (left[i] + right[i]);
            const float side = 0.5f * (left[i] - right[i]) * sideGain;
            left[i] = mid + side;
            right[i] = mid - side;
        }
    }
    for (int c = channels; c < buffer.getNumChannels(); ++c)
        buffer.clear(c, 0, buffer.getNumSamples());
}

juce::AudioProcessorEditor* X1LEAudioProcessor::createEditor() { return new X1LEAudioProcessorEditor(*this); }
void X1LEAudioProcessor::getStateInformation(juce::MemoryBlock& dest) {
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void X1LEAudioProcessor::setStateInformation(const void* data, int size) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new X1LEAudioProcessor(); }
