/*
  ==============================================================================

    Oscillator.h


  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "PluginProcessor.h"


class Oscillator : public juce::Component,
				   public juce::ComboBox::Listener,
				   public juce::Slider::Listener
{
public:
	Oscillator(JVSTAudioProcessor&);
	~Oscillator();

	void paint(juce::Graphics&) override;
	void resized() override;

private:
	juce::ComboBox oscMenu;
	juce::ComboBox numVoices;
	juce::ComboBox::Listener* boxState;

	juce::Slider deltaFreq;
	juce::Slider::Listener* slider;

	void comboBoxChanged(juce::ComboBox* boxState) override;

	void sliderValueChanged(juce::Slider* slider) override;

	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	JVSTAudioProcessor& processor;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscillator)
};