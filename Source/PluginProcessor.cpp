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
    add("presence","Presence",0.0f,12.0f,2.0f);
    add("air","Air",0.0f,12.0f,1.5f);
    add("comp","Comp",0.0f,1.0f,0.35f);
    add("deess","De-ess",0.0f,1.0f,0.25f);
    add("drive","Drive",0.0f,1.0f,0.08f);
    add("mix","Mix",0.0f,1.0f,1.0f);
    add("output","Output",-18.0f,6.0f,0.0f);
    return p;
}
void X1LEAudioProcessor::prepareToPlay(double sr, int) {
    juce::dsp::ProcessSpec spec{sr, 512, (juce::uint32)getTotalNumOutputChannels()};
    lowCutL.prepare(spec); lowCutR.prepare(spec); presenceL.prepare(spec); presenceR.prepare(spec);
    lowCutL.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 85.0);
    lowCutR.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 85.0);
    presenceL.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, 3200.0, 0.8f, 1.25f);
    presenceR.coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sr, 3200.0, 0.8f, 1.25f);
    envelope = 0.0f;
}
void X1LEAudioProcessor::releaseResources() {}
void X1LEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    const int channels = juce::jmin(2, buffer.getNumChannels());
    const float in = juce::Decibels::decibelsToGain(*parameters.getRawParameterValue("input"));
    const float out = juce::Decibels::decibelsToGain(*parameters.getRawParameterValue("output"));
    const float gate = *parameters.getRawParameterValue("gate");
    const float comp = *parameters.getRawParameterValue("comp");
    const float drive = *parameters.getRawParameterValue("drive");
    const float mix = *parameters.getRawParameterValue("mix");
    const float air = *parameters.getRawParameterValue("air") / 12.0f;
    const float presence = *parameters.getRawParameterValue("presence") / 12.0f;
    const float deess = *parameters.getRawParameterValue("deess");
    for (int c=0;c<channels;++c) {
        auto* x = buffer.getWritePointer(c);
        auto& hp = c==0 ? lowCutL : lowCutR;
        auto& pr = c==0 ? presenceL : presenceR;
        for (int i=0;i<buffer.getNumSamples();++i) {
            const float dry = x[i];
            float y = hp.processSample(dry * in);
            y = pr.processSample(y);
            const float a = std::abs(y);
            envelope = 0.995f * envelope + 0.005f * a;
            const float gateGain = envelope < gate * 0.10f ? 0.35f : 1.0f;
            const float compressed = y / (1.0f + comp * std::max(0.0f, std::abs(y) - 0.22f) * 5.0f);
            const float shaped = std::tanh(compressed * (1.0f + drive * 3.0f));
            float bright = shaped * (1.0f + presence * 0.12f + air * 0.08f);
            bright *= (1.0f - deess * juce::jlimit(0.0f,0.45f,(a-0.45f)*0.4f));
            const float wet = bright * gateGain;
            x[i] = (dry * (1.0f-mix) + wet * mix) * out;
        }
    }
    for (int c=channels;c<buffer.getNumChannels();++c) buffer.clear(c,0,buffer.getNumSamples());
}
juce::AudioProcessorEditor* X1LEAudioProcessor::createEditor() { return new X1LEAudioProcessorEditor(*this); }
void X1LEAudioProcessor::getStateInformation(juce::MemoryBlock& dest) {
    auto state = parameters.copyState(); std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void X1LEAudioProcessor::setStateInformation(const void* data, int size) {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data,size));
    if (xml && xml->hasTagName(parameters.state.getType())) parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new X1LEAudioProcessor(); }
