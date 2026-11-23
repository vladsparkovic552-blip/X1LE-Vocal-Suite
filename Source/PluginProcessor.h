#pragma once
#include <JuceHeader.h>

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
    float envelope = 0.0f;
    juce::dsp::IIR::Filter<float> lowCutL, lowCutR, presenceL, presenceR;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(X1LEAudioProcessor)
};
