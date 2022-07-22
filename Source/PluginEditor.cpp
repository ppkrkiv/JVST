/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


 
//==============================================================================
JVSTAudioProcessorEditor::JVSTAudioProcessorEditor (JVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), oscGui(p), envGui(p),
	  filterGui(p), keyboardComponent(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 400); // Sets the size of the window

	// set each components variables
	masterVol.setSliderStyle(juce::Slider::Rotary);
	masterVol.setRange(-64, 16, 1);
	masterVol.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
	masterVol.setPopupDisplayEnabled(true, false, this);
	masterVol.setTextValueSuffix(" dB");
	masterVol.setSkewFactorFromMidPoint(-32);
	masterVol.setValue(1.0);

	toggleFX.addListener(this);

	audioProcessor.waveViewer.setColours(juce::Colour (0xff2b2c30), juce::Colours::deepskyblue);

	modWheel.setSliderStyle(juce::Slider::Rotary);
	modWheel.setRange(0, 1, double(1 / 128));
	modWheel.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
	modWheel.setValue(0.0);

	keyboardComponent.setColour(keyboardComponent.blackNoteColourId, juce::Colours::black);
	keyboardComponent.setColour(keyboardComponent.whiteNoteColourId, juce::Colours::whitesmoke);

	//add listeners to main component sliders
	masterVol.addListener(this);
	modWheel.addListener(this);

	//add components to the main component
	addAndMakeVisible(toggleFX);
	addAndMakeVisible(&modWheel);
	addAndMakeVisible(&masterVol);
	addAndMakeVisible(&oscGui);
	addAndMakeVisible(&envGui);
	addAndMakeVisible(&filterGui);
	addAndMakeVisible(audioProcessor.waveViewer);
	addAndMakeVisible(keyboardComponent);

}

JVSTAudioProcessorEditor::~JVSTAudioProcessorEditor()
{
	setOpaque(false);
}

//==============================================================================
void JVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Settings for background, font
	g.fillAll (juce::Colour (0xff32333a));
    g.setColour (juce::Colours::lightgrey);
    g.setFont (14.0f);
	g.setFont(juce::Font("Courier", 12.0f, juce::Font::plain));
	g.drawText("Volume", 405, 255, 50, 20, juce::Justification::centred);
	g.drawText("Mod", 335, 255, 50, 20, juce::Justification::centred);
	g.drawText("FX", 230, 255, 50, 20, juce::Justification::centred);
}

void JVSTAudioProcessorEditor::resized()
{
	// sets position and size for each component
	juce::Rectangle<int> area = getLocalBounds();

	const int componentWidth = 200;
	const int componentHeight = 200;

	toggleFX.setBounds(240, 220, 30, 30);

	juce::Rectangle<int> extArea(20, 50, 20, 100);
	bendExtension.setBounds(extArea);
	pitchWheel.setBounds(300, 200, 60, 60);

	masterVol.setBounds(400, 200, 60, 60);
	modWheel.setBounds(330, 200, 60, 60);
	keyboardComponent.setBounds(area.removeFromBottom(componentHeight / 1.8));

	oscGui.setBounds(8, 20, 200, 200);
	envGui.setBounds(8, 125, 200, 200);
	filterGui.setBounds(600, 50, 200, 200);
	audioProcessor.waveViewer.setBounds(220, 20, 365, 170);
	//fxMenu.setBounds(205, 225, 100, 20);
}

void JVSTAudioProcessorEditor::buttonClicked(juce::Button* button)
{
	if (button == &toggleFX)
	{
		audioProcessor.fxSelected++;
	}
}

void JVSTAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
	audioProcessor.masterVolume = std::pow(10, masterVol.getValue() / 20);
	audioProcessor.pitchWheel = pitchWheel.getValue();
	audioProcessor.modWheel = modWheel.getValue();
	audioProcessor.bendExt = bendExtension.getValue();
}
