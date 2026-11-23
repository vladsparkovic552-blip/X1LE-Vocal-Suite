#include "PluginEditor.h"

namespace {
    const juce::Colour bg(0xff070912), panel(0xff101728), panel2(0xff151f33);
    const juce::Colour border(0xff283a59), purple(0xffa66bff), cyan(0xff43e5ff);
    const juce::Colour white(0xfff3f6ff), muted(0xff8796b3);

    juce::Rectangle<float> moduleBounds(int index, int width) {
        const float margin = 24.0f, gap = 14.0f;
        const float cardW = (static_cast<float>(width) - margin * 2.0f - gap * 2.0f) / 3.0f;
        const float cardH = 148.0f;
        return { margin + (index % 3) * (cardW + gap),
                 132.0f + (index / 3) * (cardH + 13.0f), cardW, cardH };
    }
}

X1LEAudioProcessorEditor::X1LEAudioProcessorEditor(X1LEAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(980, 650);
    setResizable(true, true);
    setResizeLimits(820, 600, 1280, 850);

    presetLabel.setText("VOCAL PRESET", juce::dontSendNotification);
    presetLabel.setColour(juce::Label::textColourId, muted);
    presetLabel.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
    addAndMakeVisible(presetLabel);
    presetBox.addItem("Clean Studio", 1);
    presetBox.addItem("Modern Rap", 2);
    presetBox.addItem("Melodic Vocal", 3);
    presetBox.addItem("Bright Pop", 4);
    presetBox.addItem("Airy Vocal", 5);
    presetBox.addItem("Hard Vocal", 6);
    presetBox.addItem("Wide / Flanger", 7);
    presetBox.addItem("Dreamy Space", 8);
    presetBox.addItem("Warm Low Cut", 9);
    presetBox.setSelectedId(1, juce::dontSendNotification);
    presetBox.setColour(juce::ComboBox::backgroundColourId, panel2);
    presetBox.setColour(juce::ComboBox::textColourId, white);
    presetBox.setColour(juce::ComboBox::outlineColourId, border);
    presetBox.onChange = [this] { applyPreset(presetBox.getSelectedId()); };
    addAndMakeVisible(presetBox);

    for (int i = 0; i < 9; ++i) {
        auto& s = controls[i];
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 78, 20);
        s.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                              juce::MathConstants<float>::pi * 2.75f, true);
        s.setNumDecimalPlacesToDisplay(1);
        s.setColour(juce::Slider::rotarySliderFillColourId, i % 2 == 0 ? purple : cyan);
        s.setColour(juce::Slider::rotarySliderOutlineColourId, border);
        s.setColour(juce::Slider::thumbColourId, white);
        s.setColour(juce::Slider::textBoxTextColourId, white);
        s.setColour(juce::Slider::textBoxBackgroundColourId, bg);
        s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        s.setColour(juce::Slider::textBoxHighlightColourId, purple);
        s.setTooltip(juce::String(names[i]) + ": " + descriptions[i]);
        addAndMakeVisible(s);
        attachments[i] = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters, ids[i], s);
    }
    for (int i : { 0, 3, 4, 8 }) controls[i].setTextValueSuffix(" dB");
    for (int i : { 1, 2, 5, 6, 7 }) {
        controls[i].textFromValueFunction = [](double v) { return juce::String(juce::roundToInt(v * 100.0)) + " %"; };
        controls[i].valueFromTextFunction = [](const juce::String& v) { return v.getFloatValue() / 100.0; };
    }
}

void X1LEAudioProcessorEditor::applyPreset(int preset) {
    struct Values { float input, gate, comp, presence, air, deess, drive, mix, output, lowcut, flanger, delay, reverb; };
    static const Values presets[] = {
        { 0.0f, 0.08f, 0.30f, 2.0f, 1.5f, 0.22f, 0.04f, 1.0f, 0.0f, 85.0f, 0.00f, 0.00f, 0.04f },
        { -2.0f, 0.05f, 0.55f, 3.8f, 2.0f, 0.30f, 0.12f, 1.0f, -1.0f, 100.0f, 0.04f, 0.10f, 0.03f },
        { -1.0f, 0.04f, 0.42f, 2.5f, 3.0f, 0.28f, 0.05f, 1.0f, -0.5f, 90.0f, 0.16f, 0.20f, 0.28f },
        { -1.0f, 0.06f, 0.38f, 3.0f, 3.8f, 0.35f, 0.03f, 1.0f, -1.0f, 110.0f, 0.02f, 0.08f, 0.10f },
        { -2.0f, 0.03f, 0.25f, 1.8f, 5.0f, 0.25f, 0.02f, 1.0f, -1.0f, 120.0f, 0.08f, 0.18f, 0.24f },
        { -3.0f, 0.10f, 0.68f, 4.0f, 1.5f, 0.38f, 0.18f, 1.0f, -2.0f, 95.0f, 0.05f, 0.06f, 0.02f },
        { -2.0f, 0.04f, 0.45f, 3.0f, 2.5f, 0.30f, 0.07f, 1.0f, -1.0f, 100.0f, 0.48f, 0.12f, 0.10f },
        { -2.0f, 0.03f, 0.38f, 2.2f, 3.5f, 0.26f, 0.04f, 1.0f, -1.0f, 95.0f, 0.20f, 0.34f, 0.52f },
        { -1.0f, 0.05f, 0.42f, 2.8f, 1.8f, 0.26f, 0.05f, 1.0f, -0.5f, 145.0f, 0.00f, 0.08f, 0.08f }
    };
    const auto& v = presets[juce::jlimit(0, 8, preset - 1)];
    const float values[] = { v.input, v.gate, v.comp, v.presence, v.air, v.deess, v.drive, v.mix, v.output };
    for (int i = 0; i < 9; ++i) {
        if (auto* param = audioProcessor.parameters.getParameter(ids[i]))
            param->setValueNotifyingHost(param->convertTo0to1(values[i]));
    }
    const char* fxIds[] = { "lowcut", "flanger", "delay", "reverb" };
    const float fxValues[] = { v.lowcut, v.flanger, v.delay, v.reverb };
    for (int i = 0; i < 4; ++i)
        if (auto* param = audioProcessor.parameters.getParameter(fxIds[i]))
            param->setValueNotifyingHost(param->convertTo0to1(fxValues[i]));
}

void X1LEAudioProcessorEditor::drawModuleIcon(juce::Graphics& g, juce::Rectangle<float> a, int index) {
    const auto c = index % 2 == 0 ? purple : cyan;
    g.setColour(c.withAlpha(0.12f)); g.fillRoundedRectangle(a, 10.0f);
    g.setColour(c);
    const float x=a.getX(), y=a.getY(), w=a.getWidth(), h=a.getHeight(), cx=a.getCentreX(), cy=a.getCentreY();
    if (index == 0 || index == 2 || index == 4) {
        juce::Path p; p.startNewSubPath(x+5,cy);
        for (int n=0;n<5;++n) p.lineTo(x+8+n*5, cy + ((n%2)==0 ? -8.0f : 8.0f));
        p.lineTo(x+w-4,cy);
        g.strokePath(p, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    } else if (index == 1 || index == 5) {
        g.drawRoundedRectangle(x+7,y+7,w-14,h-14,5.0f,2.0f);
        g.drawLine(cx,y+8,cx,y+h-8,2.0f);
    } else if (index == 3 || index == 6) {
        juce::Path p; p.startNewSubPath(cx,y+5); p.lineTo(cx+4,cy-3); p.lineTo(x+w-5,cy);
        p.lineTo(cx+3,cy+3); p.lineTo(cx,y+h-5); p.lineTo(cx-3,cy+3);
        p.lineTo(x+5,cy); p.closeSubPath(); g.strokePath(p,juce::PathStrokeType(2.0f));
    } else {
        g.drawEllipse(x+5,y+7,w-10,h-14,2.0f);
        g.drawLine(x+9,cy,x+w-9,cy,2.0f);
    }
}

void X1LEAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(bg);
    juce::ColourGradient background(juce::Colour(0xff1a1640),0,0,juce::Colour(0xff09101c),getWidth(),getHeight(),false);
    g.setGradientFill(background); g.fillRect(getLocalBounds());
    g.setColour(purple.withAlpha(0.12f)); g.fillEllipse(getWidth()-280.0f,-170.0f,420.0f,420.0f);
    g.setColour(cyan.withAlpha(0.07f)); g.fillEllipse(-180.0f,350.0f,380.0f,380.0f);

    g.setColour(panel.withAlpha(0.96f)); g.fillRoundedRectangle(18.0f,16.0f,getWidth()-36.0f,100.0f,18.0f);
    g.setColour(purple); g.fillRoundedRectangle(18.0f,16.0f,4.0f,100.0f,2.0f);
    g.setColour(white); g.setFont(juce::Font(juce::FontOptions(28.0f,juce::Font::bold)));
    g.drawText("X1LE",38,28,95,34,juce::Justification::centredLeft);
    g.setColour(cyan); g.drawText("VOCAL SUITE",132,30,220,30,juce::Justification::centredLeft);
    g.setColour(muted); g.setFont(juce::Font(juce::FontOptions(10.5f,juce::Font::bold)));
    g.drawText("NEURAL-INSPIRED VOCAL COLOR  /  REAL-TIME CHAIN",40,70,380,18,juce::Justification::centredLeft);
    g.setColour(cyan.withAlpha(0.14f)); g.fillRoundedRectangle(getWidth()-190.0f,42.0f,148.0f,30.0f,10.0f);
    g.setColour(cyan); g.fillEllipse(getWidth()-176.0f,53.0f,8.0f,8.0f);
    g.setColour(white); g.setFont(juce::Font(juce::FontOptions(10.0f,juce::Font::bold)));
    g.drawText("AUDIO ENGINE ON",getWidth()-162,48,112,18,juce::Justification::centredLeft);

    for (int i=0;i<9;++i) {
        auto r=moduleBounds(i,getWidth());
        g.setColour(panel.withAlpha(0.98f)); g.fillRoundedRectangle(r,15.0f);
        g.setColour(border); g.drawRoundedRectangle(r,15.0f,1.0f);
        g.setColour((i%2==0?purple:cyan).withAlpha(0.9f));
        g.fillRoundedRectangle(r.getX()+1,r.getY()+15,3,38,1.5f);
        drawModuleIcon(g,{r.getX()+15,r.getY()+15,36,36},i);
        g.setColour(white); g.setFont(juce::Font(juce::FontOptions(11.0f,juce::Font::bold)));
        g.drawText(names[i],juce::Rectangle<int>((int)r.getX()+60,(int)r.getY()+17,132,20),juce::Justification::centredLeft,true);
        g.setColour(muted); g.setFont(juce::Font(juce::FontOptions(9.5f)));
        g.drawFittedText(descriptions[i],juce::Rectangle<int>((int)r.getX()+15,(int)r.getY()+65,132,70),juce::Justification::topLeft,4,1.0f);
    }
    g.setColour(muted); g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("START WITH A PRESET, THEN ADJUST BY EAR. KEEP INPUT AND OUTPUT LEVELS UNDER CONTROL.",
               25,getHeight()-25,getWidth()-50,16,juce::Justification::centredLeft);
}

void X1LEAudioProcessorEditor::resized() {
    presetLabel.setBounds(440,34,105,18);
    presetBox.setBounds(440,55,190,30);
    for (int i=0;i<9;++i) {
        const auto r=moduleBounds(i,getWidth());
        controls[i].setBounds((int)(r.getRight()-126.0f),(int)(r.getY()+24.0f),114,116);
    }
}
