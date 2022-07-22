/*
  ==============================================================================

    Oscillator.cpp


  ==============================================================================
*/

#include "Oscillator.h"
#include "JuceHeader.h"

Oscillator::Oscillator(JVSTAudioProcessor& p) :
	processor(p)
{
	//size of the whole component
	setSize(200, 200);

	//drop down menu attributes
	oscMenu.addItem("Saw", 1);
	oscMenu.addItem("Sine", 2);
	oscMenu.addItem("Square", 3);
	oscMenu.addItem("Triangle", 4);
	oscMenu.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(&oscMenu);

	numVoices.addItem("1 voice", 1);
	numVoices.addItem("2 voices", 2);
	numVoices.addItem("3 voices", 3);
	numVoices.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(&numVoices);

	//add listeners and set the first option as active
	numVoices.setSelectedId(1);
	numVoices.addListener(this);
	oscMenu.setSelectedId(1);
	oscMenu.addListener(this);

	deltaFreq.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	deltaFreq.setRange(0.1f, 20.0f);
	deltaFreq.setValue(10.0f);
	deltaFreq.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
	addAndMakeVisible(&deltaFreq);
	deltaFreq.addListener(this);

}

Oscillator::~Oscillator()
{
}

void Oscillator::paint(juce::Graphics& g)
{
	juce::Rectangle<int> titleArea(0, 10, getWidth(), 20);
	g.setColour(juce::Colours::lightgrey);
	g.setFont(juce::Font("Courier", 12.0f, juce::Font::plain));
	g.drawText("Sounds", titleArea, juce::Justification::centredTop);
	g.drawText("Delta", 80, 110, 40, 20, juce::Justification::centred);
}

void Oscillator::resized()
{
	juce::Rectangle<int> area = getLocalBounds().reduced(40);
	oscMenu.setBounds(area.removeFromTop(20));
	numVoices.setBounds(area.removeFromTop(20));
	deltaFreq.setBounds(area.removeFromTop(50));
}

// when different oscillator is selected
void Oscillator::comboBoxChanged(juce::ComboBox* boxThatChanged)
{
	processor.oscBoxSelected = oscMenu.getSelectedId();
	processor.numVoices = numVoices.getSelectedId();
	processor.initialiseSynth();
}

void Oscillator::sliderValueChanged(juce::Slider* slider)
{
	processor.deltaFrequency = deltaFreq.getValue();
}