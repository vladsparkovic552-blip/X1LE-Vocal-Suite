#pragma once
#include "PluginProcessor.h"

class X1LEAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit X1LEAudioProcessorEditor(X1LEAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    X1LEAudioProcessor& audioProcessor;
    juce::Label title, subtitle;
    juce::Slider controls[9];
    juce::Label labels[9];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachments[9];
    const char* ids[9] = {"input","gate","presence","air","comp","deess","drive","mix","output"};
    const char* names[9] = {"INPUT","GATE","PRESENCE","AIR","COMP","DE-ESS","DRIVE","MIX","OUTPUT"};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(X1LEAudioProcessorEditor)
};
