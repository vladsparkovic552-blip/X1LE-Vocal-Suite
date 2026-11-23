#include "PluginEditor.h"

namespace {
    const juce::Colour bg       (0xff090b12);
    const juce::Colour panel    (0xff121725);
    const juce::Colour panelTop (0xff192033);
    const juce::Colour border  (0xff29334b);
    const juce::Colour purple  (0xff9a70ff);
    const juce::Colour cyan    (0xff55d7f2);
    const juce::Colour text    (0xfff2f4ff);
    const juce::Colour muted   (0xff9ba7bf);

    juce::Rectangle<float> moduleBounds(int index, int width) {
        const float margin = 24.0f;
        const float gap = 14.0f;
        const float cardW = (static_cast<float>(width) - margin * 2.0f - gap * 2.0f) / 3.0f;
        const float cardH = 145.0f;
        const float x = margin + (index % 3) * (cardW + gap);
        const float y = 112.0f + (index / 3) * (cardH + 13.0f);
        return { x, y, cardW, cardH };
    }
}

X1LEAudioProcessorEditor::X1LEAudioProcessorEditor(X1LEAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(920, 610);
    setResizable(true, true);
    setResizeLimits(760, 560, 1200, 820);

    for (int i = 0; i < 9; ++i) {
        auto& slider = controls[i];
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 76, 19);
        slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                                   juce::MathConstants<float>::pi * 2.75f, true);
        slider.setNumDecimalPlacesToDisplay(1);
        slider.setColour(juce::Slider::rotarySliderFillColourId, purple);
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff303b55));
        slider.setColour(juce::Slider::thumbColourId, cyan);
        slider.setColour(juce::Slider::textBoxTextColourId, text);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff0d1220));
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0x00000000));
        slider.setColour(juce::Slider::textBoxHighlightColourId, purple);
        slider.setTooltip(juce::String(names[i]) + ": " + descriptions[i]);
        addAndMakeVisible(slider);
        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            p.parameters, ids[i], slider);
    }

    controls[0].setTextValueSuffix(" dB");
    controls[3].setTextValueSuffix(" dB");
    controls[4].setTextValueSuffix(" dB");
    controls[8].setTextValueSuffix(" dB");
    for (int index : { 1, 2, 5, 6, 7 }) {
        controls[index].textFromValueFunction = [](double value) {
            return juce::String(juce::roundToInt(value * 100.0)) + " %";
        };
        controls[index].valueFromTextFunction = [](const juce::String& value) {
            return value.getFloatValue() / 100.0;
        };
    }
}

void X1LEAudioProcessorEditor::drawModuleIcon(juce::Graphics& g, juce::Rectangle<float> area, int index) {
    const auto c = (index % 2 == 0) ? purple : cyan;
    g.setColour(c.withAlpha(0.13f));
    g.fillRoundedRectangle(area, 9.0f);
    g.setColour(c);
    const float x = area.getX(), y = area.getY(), w = area.getWidth(), h = area.getHeight();
    const float cx = area.getCentreX(), cy = area.getCentreY();

    if (index == 0) { // Input: waveform
        juce::Path p;
        p.startNewSubPath(x + 5, cy);
        p.lineTo(x + 10, cy - 5); p.lineTo(x + 14, cy + 6);
        p.lineTo(x + 19, cy - 9); p.lineTo(x + 24, cy + 3);
        p.lineTo(x + 29, cy);
        g.strokePath(p, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    } else if (index == 1) { // Gate
        g.drawRoundedRectangle(x + 7, y + 7, w - 14, h - 14, 4.0f, 2.0f);
        g.drawLine(cx, y + 8, cx, y + h - 8, 2.0f);
    } else if (index == 2) { // Compressor
        g.drawLine(x + 8, y + h - 8, x + 8, y + 8, 1.5f);
        g.drawLine(x + 8, y + h - 8, x + w - 7, y + h - 8, 1.5f);
        juce::Path p; p.startNewSubPath(x + 11, y + h - 12);
        p.lineTo(cx - 2, cy + 1); p.lineTo(x + w - 9, y + 11);
        g.strokePath(p, juce::PathStrokeType(2.2f));
    } else if (index == 3) { // Presence / focus
        juce::Path p; p.startNewSubPath(cx, y + 6); p.lineTo(cx + 3, cy - 3);
        p.lineTo(x + w - 6, cy); p.lineTo(cx + 3, cy + 3);
        p.lineTo(cx, y + h - 6); p.lineTo(cx - 3, cy + 3);
        p.lineTo(x + 6, cy); p.lineTo(cx - 3, cy - 3); p.closeSubPath();
        g.fillPath(p);
    } else if (index == 4) { // Air: breeze
        for (int k = 0; k < 3; ++k) {
            juce::Path p; const float yy = y + 10 + k * 8.0f;
            p.startNewSubPath(x + 7, yy); p.cubicTo(x + 14, yy - 5, x + 22, yy + 5, x + 29, yy);
            g.strokePath(p, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
    } else if (index == 5) { // De-esser
        g.setFont(juce::Font(juce::FontOptions(20.0f, juce::Font::bold)));
        g.drawText("S", area.toNearestInt(), juce::Justification::centred, false);
    } else if (index == 6) { // Drive: bolt
        juce::Path p; p.startNewSubPath(cx + 2, y + 5); p.lineTo(x + 10, cy + 2);
        p.lineTo(cx - 1, cy + 2); p.lineTo(cx - 3, y + h - 5);
        p.lineTo(x + w - 9, cy - 4); p.lineTo(cx + 2, cy - 4); p.closeSubPath();
        g.fillPath(p);
    } else if (index == 7) { // Mix
        g.drawEllipse(x + 6, y + 9, 17, 17, 2.0f);
        g.drawEllipse(x + 15, y + 9, 17, 17, 2.0f);
    } else { // Output
        juce::Path p; p.startNewSubPath(x + 7, cy - 5); p.lineTo(x + 15, cy - 5);
        p.lineTo(x + 25, y + 8); p.lineTo(x + 25, y + h - 8);
        p.lineTo(x + 15, cy + 5); p.lineTo(x + 7, cy + 5); p.closeSubPath();
        g.strokePath(p, juce::PathStrokeType(2.0f));
        juce::Path soundWaves;
        soundWaves.addCentredArc(x + 27.0f, cy, 8.0f, (h - 14.0f) * 0.5f,
                                 0.0f, -0.9f, 0.9f, true);
        g.strokePath(soundWaves, juce::PathStrokeType(1.8f));
    }
}

void X1LEAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(bg);

    // Header background and accent
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff1a2035), 0.0f, 0.0f,
                                           juce::Colour(0xff101522), 0.0f, 100.0f, false));
    g.fillRoundedRectangle(18.0f, 16.0f, getWidth() - 36.0f, 82.0f, 16.0f);
    g.setColour(purple);
    g.fillRoundedRectangle(18.0f, 16.0f, 4.0f, 82.0f, 2.0f);

    g.setColour(text);
    g.setFont(juce::Font(juce::FontOptions(25.0f, juce::Font::bold)));
    g.drawText("X1LE", 38, 27, 86, 34, juce::Justification::centredLeft);
    g.setColour(purple);
    g.drawText("VOCAL SUITE", 119, 29, 190, 30, juce::Justification::centredLeft);
    g.setColour(muted);
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.drawText("VOCAL CHAIN  /  REAL-TIME PROCESSING", 40, 62, 350, 18, juce::Justification::centredLeft);

    // Status chip
    g.setColour(juce::Colour(0xff172d32));
    g.fillRoundedRectangle(getWidth() - 184.0f, 42.0f, 142.0f, 28.0f, 9.0f);
    g.setColour(cyan);
    g.fillEllipse(getWidth() - 169.0f, 52.0f, 8.0f, 8.0f);
    g.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
    g.drawText("PLUGIN ACTIVE", getWidth() - 154, 46, 104, 18, juce::Justification::centredLeft);

    // Module cards
    for (int i = 0; i < 9; ++i) {
        auto r = moduleBounds(i, getWidth());
        g.setColour(panel);
        g.fillRoundedRectangle(r, 13.0f);
        g.setColour(border);
        g.drawRoundedRectangle(r, 13.0f, 1.0f);
        g.setColour((i % 2 == 0) ? purple.withAlpha(0.8f) : cyan.withAlpha(0.8f));
        g.fillRoundedRectangle(r.getX() + 1.0f, r.getY() + 14.0f, 3.0f, 36.0f, 1.5f);

        drawModuleIcon(g, { r.getX() + 15.0f, r.getY() + 14.0f, 36.0f, 36.0f }, i);

        g.setColour(text);
        g.setFont(juce::Font(juce::FontOptions(11.5f, juce::Font::bold)));
        g.drawText(names[i], juce::Rectangle<int>(
            static_cast<int>(r.getX() + 60.0f), static_cast<int>(r.getY() + 13.0f), 125, 20),
            juce::Justification::centredLeft, true);

        g.setColour(muted);
        g.setFont(juce::Font(juce::FontOptions(10.0f)));
        g.drawFittedText(descriptions[i], juce::Rectangle<int>(
            static_cast<int>(r.getX() + 16), static_cast<int>(r.getY() + 59),
            132, 72), juce::Justification::topLeft, 4, 1.0f);
    }

    // Footer
    g.setColour(muted);
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("TIP: начинайте с INPUT и GATE, затем настройте COMP и TONE. Сравнивайте результат с исходным сигналом.",
               25, getHeight() - 25, getWidth() - 50, 16, juce::Justification::centredLeft);
}

void X1LEAudioProcessorEditor::resized() {
    for (int i = 0; i < 9; ++i) {
        const auto r = moduleBounds(i, getWidth());
        controls[i].setBounds(static_cast<int>(r.getRight() - 124.0f),
                              static_cast<int>(r.getY() + 22.0f), 112, 116);
    }
}
