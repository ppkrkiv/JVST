/*
  ==============================================================================
  handles the ui of the filter component
  ==============================================================================
*/

#include "Filter.h"
#include "JuceHeader.h"

Filter::Filter(JVSTAudioProcessor& p) :
	processor(p)
{
	// setting all the components attributes (for gui)
	setSize(200, 300);

	filterMenu.addItem("Low Pass ", 1);
	filterMenu.addItem("High Pass ", 2);
	filterMenu.addItem("Band Pass ", 3);

	filterKind.addItem("State Variable Filter", 1);
	filterKind.addItem("IIR", 2);
	filterKind.addItem("FIR", 3);

	filterMenu.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(&filterMenu);

	filterKind.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(&filterKind);

	filterCutoff.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	filterCutoff.setRange(20.0, 10000.0, 1.0);
	filterCutoff.setValue(400.0);
	filterCutoff.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	filterCutoff.setPopupDisplayEnabled(true, true, this);
	filterCutoff.setSkewFactorFromMidPoint(1000);
	addAndMakeVisible(&filterCutoff);

	filterRes.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	filterRes.setRange(1.0, 5.0, 0.1);
	filterRes.setValue(1);
	filterRes.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	filterRes.setSkewFactorFromMidPoint(2.5);
	filterRes.setPopupDisplayEnabled(true, true, this);
	addAndMakeVisible(&filterRes);

	filterCutoff.addListener(this);

	filterTypeVal = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(processor.tree, "filterType", filterMenu);
	filterKindVal = new juce::AudioProcessorValueTreeState::ComboBoxAttachment(processor.tree, "filterKind", filterKind);
	resVal = new juce::AudioProcessorValueTreeState::SliderAttachment(processor.tree, "filterRes", filterRes);
}

Filter::~Filter()
{
	filterCutoff.setLookAndFeel(nullptr);
	filterRes.setLookAndFeel(nullptr);
}

void Filter::paint(juce::Graphics& g)
{
	juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
	g.setColour(juce::Colours::lightgrey);
	g.setFont(juce::Font("Courier", 12.0f, juce::Font::plain));
	g.drawText("Filter", titleArea, juce::Justification::centredTop);
	g.drawText("Cutoff", 42, 170, 40, 20, juce::Justification::centred);
	g.drawText("Res", 112, 170, 40, 20, juce::Justification::centred);
}

//position the components
void Filter::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);
	filterMenu.setBounds(area.removeFromTop(20));
	filterKind.setBounds(area.removeFromTop(40));
	filterCutoff.setBounds(30, 110, 65, 65);
	filterRes.setBounds(100, 110, 65, 65);
}

void Filter::sliderValueChanged(juce::Slider* slider)
{
	processor.cutoff = filterCutoff.getValue();
}