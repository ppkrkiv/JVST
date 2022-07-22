/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Saw.h"
#include "Sine.h"
#include "Square.h"
#include "Triangle.h"

//==============================================================================
/**
*/
class JVSTAudioProcessor  : public juce::AudioProcessor,
							public juce::MidiKeyboardStateListener
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    JVSTAudioProcessor();
    ~JVSTAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================
	juce::AudioProcessorValueTreeState tree;

	void updateFilter();
	void process(juce::dsp::ProcessContextReplacing<float> context);
	int oscBoxSelected;
	double deltaFrequency;
	int numVoices;
	int bendExt;
	int fxSelected = 0;
	void initialiseSynth();
	double cutoff;
	double masterVolume;
	double pitchWheel;
	double modWheel;

	juce::AudioVisualiserComponent waveViewer;

	float noteOnVel;
	juce::MidiKeyboardState keyboardState;

private:
    //==============================================================================
	juce::Synthesiser jSynth;
	SawWaveVoice* jSaw;
	SineWaveVoice* jSine;
	SquareWaveVoice* jSquare;
	TriangleWaveVoice* jTriangle;
	juce::AudioProcessorEditor* editor = getActiveEditor();

	void handleNoteOn(juce::MidiKeyboardState*, int midichannel, int midiNoteNumber, float velocity) override;
	void handleNoteOff(juce::MidiKeyboardState*, int midichannel, int midiNoteNumber, float velocity) override;
	void handlePitchWheel(juce::MidiKeyboardState*, int midiChannel, int wheelValue);
	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message);

	void setMidiInput(int index);
	int lastInputIndex;
	bool isAddingFromMidiInput;
	double lastSampleRate;



	juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableFilter::Filter <float>, juce::dsp::StateVariableFilter::Parameters <float>> stateVariableFilter;
	juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter <float>, juce::dsp::IIR::Coefficients <float>> IIRFilter;
	juce::dsp::ProcessorDuplicator<juce::dsp::FIR::Filter <float>, juce::dsp::FIR::Coefficients <float>> FIRFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JVSTAudioProcessor)
};
