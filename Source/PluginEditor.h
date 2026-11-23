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

    const char* ids[9] = { "input", "gate", "comp", "presence", "air", "deess", "drive", "mix", "output" };
    const char* names[9] = { "INPUT GAIN", "GATE", "COMPRESSION", "PRESENCE", "AIR", "DE-ESSER", "DRIVE", "MIX", "OUTPUT" };
    const char* descriptions[9] = {
        "Уровень сигнала до обработки. Настройте так, чтобы вход не перегружался.",
        "Прибирает тихие участки между фразами. Слишком высокое значение обрежет окончания слов.",
        "Сглаживает разницу между тихими и громкими частями вокала.",
        "Добавляет разборчивость и выдвигает голос вперёд.",
        "Добавляет яркость верхним частотам. Используйте умеренно.",
        "Смягчает резкие сибилянты и излишне яркие звуки «с» и «ш».",
        "Добавляет насыщение и гармоники. Повышайте постепенно.",
        "Баланс исходного и обработанного сигнала.",
        "Финальная громкость после обработки. Следите, чтобы не было клиппинга."
    };

    void drawModuleIcon(juce::Graphics&, juce::Rectangle<float>, int);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(X1LEAudioProcessorEditor)
};
