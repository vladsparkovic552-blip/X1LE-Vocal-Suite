#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>

class X1LEAudioProcessor : public juce::AudioProcessor {
public:
    X1LEAudioProcessor();
    ~X1LEAudioProcessor() override = default;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "X1LE Vocal Suite"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 1.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;
    juce::AudioProcessorValueTreeState parameters;
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    std::array<juce::dsp::IIR::Filter<float>, 2> highPass, presence, airShelf, essHighPass, essLowPass;
    std::array<float, 2> compEnvelope { 0.0f, 0.0f };
    std::array<float, 2> essEnvelope { 0.0f, 0.0f };
    float gateEnvelope = 0.0f;
    std::array<float, 2> gateGain { 1.0f, 1.0f };
    std::array<std::vector<float>, 2> flangeBuffer, delayBuffer;
    int flangeWrite = 0, delayWrite = 0;
    double flangePhase = 0.0;
    float currentLowCut = 85.0f;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(X1LEAudioProcessor)
};
