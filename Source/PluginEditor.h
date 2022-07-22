/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "PluginProcessor.h"
#include "Oscillator.h"
#include "Envelope.h"
#include "Filter.h"

//==============================================================================
/**
*/
class JVSTAudioProcessorEditor : public juce::AudioProcessorEditor,
	//public juce::ComboBox::Listener,
								 public juce::Slider::Listener,
								 public juce::Button::Listener

{
public:
	JVSTAudioProcessorEditor(JVSTAudioProcessor&);
    ~JVSTAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
	void resized() override;
	float steps = 0.0f;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
	JVSTAudioProcessor& audioProcessor;
	Oscillator oscGui;
	Envelope envGui;
	Filter filterGui;
	juce::Slider masterVol;
	juce::Slider bendExtension;
	juce::Slider pitchWheel;
	juce::Slider modWheel;
	//juce::ComboBox fxMenu;
	//juce::ComboBox::Listener* fxMenuOption;
	juce::TextButton toggleFX;
	juce::Slider::Listener* slider;
	juce::TextEditor midiMessagesBox;
	juce::MidiKeyboardComponent keyboardComponent;

	double lfoFreq = 20;
	juce::Timer* timerPointer;
	double startTime;

	//void changeFx(juce::ComboBox* fxMenuState) override;
	void sliderValueChanged(juce::Slider* slider) override; // volume slider
	void buttonClicked(juce::Button* button) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JVSTAudioProcessorEditor)
};