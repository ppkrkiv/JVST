/*
  ==============================================================================
  filter header file
  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/*
*/
class Filter : public juce::Component,
	public juce::Slider::Listener
{
public:
	Filter(JVSTAudioProcessor&);
	~Filter();

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	juce::Slider filterCutoff;
	juce::Slider filterRes;

	juce::Slider::Listener* slider;

	juce::ComboBox filterMenu;
	juce::ComboBox filterKind;

	void sliderValueChanged(juce::Slider *slider) override;

	juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterKindVal;
	juce::ScopedPointer<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeVal;
	juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> resVal;

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	JVSTAudioProcessor& processor;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Filter)
};