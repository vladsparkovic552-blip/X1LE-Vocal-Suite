#include "PluginEditor.h"

X1LEAudioProcessorEditor::X1LEAudioProcessorEditor(X1LEAudioProcessor& p)
: AudioProcessorEditor(&p), audioProcessor(p) {
    setSize(650, 410);
    title.setText("X1LE VOCAL SUITE", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(27.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colour(0xfff4f2ff));
    addAndMakeVisible(title);
    subtitle.setText("VOCAL PROCESSING  /  REAL-TIME FX", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(11.0f)));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffa49abf));
    addAndMakeVisible(subtitle);
    for (int i=0;i<9;++i) {
        auto& s=controls[i]; s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,64,18);
        s.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff9b68ff));
        s.setColour(juce::Slider::thumbColourId, juce::Colour(0xffe6dcff));
        s.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        addAndMakeVisible(s);
        labels[i].setText(names[i],juce::dontSendNotification);
        labels[i].setJustificationType(juce::Justification::centred);
        labels[i].setColour(juce::Label::textColourId,juce::Colour(0xffc7bddb));
        labels[i].setFont(juce::Font(juce::FontOptions(10.0f,juce::Font::bold)));
        addAndMakeVisible(labels[i]);
        attachments[i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.parameters,ids[i],s);
    }
}
void X1LEAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff100d18));
    g.setColour(juce::Colour(0xff1d1729)); g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(12),18);
    g.setColour(juce::Colour(0xff9b68ff)); g.fillRoundedRectangle(juce::Rectangle<float>(26,86,598,2),1);
    g.setColour(juce::Colour(0xff292039)); g.fillRoundedRectangle(juce::Rectangle<float>(26,125,598,245),12);
}
void X1LEAudioProcessorEditor::resized() {
    title.setBounds(28,18,400,42); subtitle.setBounds(30,57,400,20);
    for(int i=0;i<9;++i) {
        int col=i%5, row=i/5;
        int x=34+col*119, y=132+row*112;
        if(i>=5) x=93+(i-5)*154;
        controls[i].setBounds(x,y,94,88);
        labels[i].setBounds(x,y-4,94,18);
    }
}
