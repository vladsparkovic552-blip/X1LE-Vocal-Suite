#pragma once
#include "PluginProcessor.h"

class X1LEAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit X1LEAudioProcessorEditor(X1LEAudioProcessor&);
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    X1LEAudioProcessor& audioProcessor;
    juce::Slider controls[9];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachments[9];
    juce::ComboBox presetBox;
    juce::Label presetLabel;
    const char* ids[9] = { "input", "gate", "comp", "presence", "air", "deess", "drive", "mix", "output" };
    const char* names[9] = { "INPUT GAIN", "GATE", "COMPRESSION", "PRESENCE", "AIR", "DE-ESSER", "DRIVE", "MIX", "OUTPUT" };
    const char* descriptions[9] = {
        "Set a healthy input level. Avoid clipping.",
        "Controls quiet gaps. Keep word endings natural.",
        "Smooths loud and quiet vocal phrases.",
        "Adds forward detail and vocal intelligibility.",
        "Adds a little high-frequency sheen.",
        "Tames sharp S and SH sounds.",
        "Adds harmonics and gentle saturation.",
        "Balances the processed and dry signal.",
        "Final output level. Keep peaks below clipping."
    };
    void applyPreset(int preset);
    void drawModuleIcon(juce::Graphics&, juce::Rectangle<float>, int);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(X1LEAudioProcessorEditor)
};
