/*
  ==============================================================================

    Envelope.h
	header file for the envelope component

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PluginProcessor.h"

class Envelope : public juce::Component
{
public:
	Envelope(JVSTAudioProcessor&);
	~Envelope();

	void paint(juce::Graphics&) override;
	void resized() override;


private:

	juce::Slider attackSlider;
	juce::Slider sustainSlider;
	juce::Slider decaySlider;
	juce::Slider releaseSlider;

	juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> attackVal;
	juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> decayVal;
	juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> sustainVal;
	juce::ScopedPointer<juce::AudioProcessorValueTreeState::SliderAttachment> releaseVal;

	// This reference is provided as a quick way for your editor
	// to access the processor object that created it 
	JVSTAudioProcessor& processor;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Envelope)
};