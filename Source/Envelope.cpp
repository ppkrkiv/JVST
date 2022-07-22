/*
  ==============================================================================
  handles the ui of the envelope component
  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"
#include "Envelope.h"

Envelope::Envelope(JVSTAudioProcessor& p) :
	processor(p)
{
	setSize(200, 200);

	//slider initialization values
	attackSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	attackSlider.setRange(0.1f, 5.0f);
	attackSlider.setValue(0.1f);
	attackSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	attackSlider.setColour(attackSlider.trackColourId, juce::Colours::greenyellow);
	attackSlider.setColour(attackSlider.textBoxOutlineColourId, juce::Colour(0x282C34));
	addAndMakeVisible(&attackSlider);

	decaySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	decaySlider.setRange(1.0f, 2.0f);
	decaySlider.setValue(1.0f);
	decaySlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	decaySlider.setColour(decaySlider.trackColourId, juce::Colours::limegreen);
	decaySlider.setColour(decaySlider.textBoxOutlineColourId, juce::Colour(0x282C34));
	addAndMakeVisible(&decaySlider);

	sustainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	sustainSlider.setRange(0.0f, 1.0f);
	sustainSlider.setValue(0.8f);
	sustainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	sustainSlider.setColour(sustainSlider.trackColourId, juce::Colours::limegreen);
	sustainSlider.setColour(sustainSlider.textBoxOutlineColourId, juce::Colour(0x282C34));
	addAndMakeVisible(&sustainSlider);

	releaseSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
	releaseSlider.setRange(0.0001f, 8.0f);
	releaseSlider.setValue(0.01f);
	releaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	releaseSlider.setColour(releaseSlider.trackColourId, juce::Colours::limegreen);
	releaseSlider.setColour(releaseSlider.textBoxOutlineColourId, juce::Colour(0x282C34));
	addAndMakeVisible(&releaseSlider);

	//sends value of the sliders to the tree state in the processor
	attackVal = new juce::AudioProcessorValueTreeState::SliderAttachment(processor.tree, "attack", attackSlider);
	decayVal = new juce::AudioProcessorValueTreeState::SliderAttachment(processor.tree, "decay", decaySlider);
	sustainVal = new juce::AudioProcessorValueTreeState::SliderAttachment(processor.tree, "sustain", sustainSlider);
	releaseVal = new juce::AudioProcessorValueTreeState::SliderAttachment(processor.tree, "release", releaseSlider);
}

Envelope::~Envelope()
{
}

void Envelope::paint(juce::Graphics& g)
{
	g.setColour(juce::Colours::lightgrey);
	g.setFont(juce::Font("Courier", 10.0f, juce::Font::plain));
	//titles
	g.drawText("Attack", 44, 80, 40, 20, juce::Justification::centredTop);
	g.drawText("Decay", 115, 80, 40, 20, juce::Justification::centredTop);
	g.drawText("Sustain", 45, 140, 40, 20, juce::Justification::centredTop);
	g.drawText("Release", 115, 140, 40, 20, juce::Justification::centredTop);
}

void Envelope::resized()
{
	//draw the sliders
	attackSlider.setBounds(30, 30, 70, 55);
	decaySlider.setBounds(100, 30, 70, 55);
	sustainSlider.setBounds(30, 90, 70, 55);
	releaseSlider.setBounds(100, 90, 70, 55);
}

