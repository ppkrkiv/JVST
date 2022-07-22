/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JVSTAudioProcessor::JVSTAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), waveViewer(2),
	tree(*this, nullptr, "PARAMETERS",
		{
			std::make_unique<juce::AudioParameterFloat>("attack", "Attack", juce::NormalisableRange<float>(0.1f, 5.0f), 0.8f),
			std::make_unique<juce::AudioParameterFloat>("decay", "Decay", juce::NormalisableRange<float>(1.0f, 2.0f), 1.0f),
			std::make_unique<juce::AudioParameterFloat>("sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f),
			std::make_unique<juce::AudioParameterFloat>("release", "Release", juce::NormalisableRange<float>(0.0001f, 8.0f), 0.01f),
			std::make_unique<juce::AudioParameterFloat>("filterType", "FilterType", juce::NormalisableRange<float>(0.0f, 2.0f), 0.0f),
			std::make_unique<juce::AudioParameterFloat>("filterKind", "FilterKind", juce::NormalisableRange<float>(0.0f, 2.0f), 0.0f),
			std::make_unique<juce::AudioParameterFloat>("filterRes", "FilterRes", juce::NormalisableRange<float>(1.0f, 5.0f, 0.1f), 1.0f),
		})
#endif
{
	keyboardState.addListener(this);
	cutoff = 400.0;
	masterVolume = 0.3;
	oscBoxSelected = 1;
	numVoices = 1;
	waveViewer.setRepaintRate(45); //visualiser freshrate
	waveViewer.setBufferSize(124); //set the buffer size
	initialiseSynth();
}

JVSTAudioProcessor::~JVSTAudioProcessor()
{
}

void JVSTAudioProcessor::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
	const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
	keyboardState.processNextMidiEvent(message);
}

void JVSTAudioProcessor::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
	jSynth.noteOn(midiChannel, midiNoteNumber, velocity);
}

void JVSTAudioProcessor::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity)
{
	jSynth.noteOff(midiChannel, midiNoteNumber, velocity, true);
}

void JVSTAudioProcessor::handlePitchWheel(juce::MidiKeyboardState*, int midiChannel, int wheelValue) {
	jSynth.handlePitchWheel(midiChannel, wheelValue);
}

//initialise the synth
void JVSTAudioProcessor::initialiseSynth()
{
	jSynth.clearVoices();
	jSynth.clearSounds();

	switch (oscBoxSelected)
	{
	case 1:
		for (int i = numVoices; --i >= 0;) {
			jSynth.addVoice(new SawWaveVoice());
		}

		jSynth.addSound(new SawWaveSound());

		break;

	case 2:
		for (int i = numVoices; --i >= 0;) {
			jSynth.addVoice(new SineWaveVoice());
		}

		jSynth.addSound(new SineWaveSound());

		break;

	case 3:
		for (int i = numVoices; --i >= 0;)
			jSynth.addVoice(new SquareWaveVoice());

		jSynth.addSound(new SquareWaveSound());

		break;

	case 4:
		for (int i = numVoices; --i >= 0;)
			jSynth.addVoice(new TriangleWaveVoice());

		jSynth.addSound(new TriangleWaveSound());

		break;

	}

}


//==============================================================================
const juce::String JVSTAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JVSTAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JVSTAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JVSTAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JVSTAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JVSTAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JVSTAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JVSTAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JVSTAudioProcessor::getProgramName (int index)
{
    return {};
}

void JVSTAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	juce::ignoreUnused(samplesPerBlock);
	lastSampleRate = sampleRate;
	jSynth.setCurrentPlaybackSampleRate(lastSampleRate);

	juce::dsp::ProcessSpec spec;   // the spec class passes the sampleRate, num channels and maximumblock size
	spec.sampleRate = lastSampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

	stateVariableFilter.reset();
	stateVariableFilter.prepare(spec);     // prepares to play for  the things to pass to the state variable filter

	IIRFilter.reset();
	IIRFilter.prepare(spec);     // prepares to play for  the things to pass to the state variable filter

	FIRFilter.reset();
	FIRFilter.prepare(spec);

	updateFilter();
}

void JVSTAudioProcessor::updateFilter()
{
	int menuChoice = *tree.getRawParameterValue("filterType");
	int res = *tree.getRawParameterValue("filterRes");

	if (menuChoice == 0)
	{
		stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff, res);

		*IIRFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, cutoff, res);

		*FIRFilter.state = *juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(cutoff, lastSampleRate, 21, juce::dsp::WindowingFunction <float>::hamming); // 21 is the filter order number 
	}
	if (menuChoice == 1)
	{
		stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff, res);

		*IIRFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, cutoff, res);

		*FIRFilter.state = *juce::dsp::FilterDesign<float>::designFIRLowpassWindowMethod(cutoff, lastSampleRate, 21, juce::dsp::WindowingFunction <float>::hamming); // 21 is the filter order number 

	}
	if (menuChoice == 2)
	{
		stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::bandPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff, res);

		*IIRFilter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(lastSampleRate, cutoff, res);
	}
}

void JVSTAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JVSTAudioProcessor::process(juce::dsp::ProcessContextReplacing<float> context)
{

}

void JVSTAudioProcessor::processBlock (juce::AudioSampleBuffer& buffer, juce::MidiBuffer& midiMessages)
{
	int filterKind = *tree.getRawParameterValue("filterKind");
	float* attack = (float*)tree.getRawParameterValue("attack");
	float* release = (float*)tree.getRawParameterValue("release");
	float* sustain = (float*)tree.getRawParameterValue("sustain");
	float* decay = (float*)tree.getRawParameterValue("decay");

	for (int i = 0; i < jSynth.getNumVoices(); i++)
	{
		switch (oscBoxSelected)
		{

		case 1:
			if ((jSaw = dynamic_cast<SawWaveVoice*>(jSynth.getVoice(i))))
			{
				jSaw->setFx(fxSelected);
				jSaw->setADSRParameters(attack, decay, sustain, release);
				jSaw->setMasterVolume(masterVolume);
				jSaw->setBendExtension(bendExt);
				jSaw->pitchWheelMoved(pitchWheel);
				jSaw->setModWheel(modWheel);
				jSaw->recalculatePitch();
			}
			break;

		case 2:
			if ((jSine = dynamic_cast<SineWaveVoice*>(jSynth.getVoice(i))))
			{
				jSine->setFx(fxSelected);
				jSine->setADSRParameters(attack, decay, sustain, release);
				jSine->setMasterVolume(masterVolume);
				jSine->setBendExtension(bendExt);
				jSine->pitchWheelMoved(pitchWheel);
				jSine->setModWheel(modWheel);
				jSine->recalculatePitch();
			}
			break;

		case 3:
			if ((jSquare = dynamic_cast<SquareWaveVoice*>(jSynth.getVoice(i))))
			{
				jSquare->setFx(fxSelected);
				jSquare->setADSRParameters(attack, decay, sustain, release);
				jSquare->setMasterVolume(masterVolume);
				jSquare->setBendExtension(bendExt);
				jSquare->pitchWheelMoved(pitchWheel);
				jSquare->setModWheel(modWheel);
				jSquare->recalculatePitch();
			}
			break;

		case 4:
			if ((jTriangle = dynamic_cast<TriangleWaveVoice*>(jSynth.getVoice(i))))
			{
				jTriangle->setFx(fxSelected);
				jTriangle->setADSRParameters(attack, decay, sustain, release);
				jTriangle->setMasterVolume(masterVolume);
				jTriangle->setBendExtension(bendExt);
				jTriangle->pitchWheelMoved(pitchWheel);
				jTriangle->setModWheel(modWheel);
				jTriangle->recalculatePitch();
			}
			break;
		}
	}

	buffer.clear();
	jSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
	updateFilter();

	juce::dsp::AudioBlock<float> block(buffer); // applies the processing to the buffer
	switch (filterKind)
	{
	case 0:
		stateVariableFilter.process(juce::dsp::ProcessContextReplacing<float>(block));   // and substitutes it into the context 
		break;
	case 1:
		IIRFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
		break;
	case 2:
		FIRFilter.process(juce::dsp::ProcessContextReplacing<float>(block));
		break;
	}

	waveViewer.pushBuffer(buffer); //push the buffer

}

//==============================================================================
bool JVSTAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JVSTAudioProcessor::createEditor()
{
    return new JVSTAudioProcessorEditor (*this);
}

//==============================================================================
void JVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JVSTAudioProcessor();
}
