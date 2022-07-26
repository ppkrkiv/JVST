/*
  ==============================================================================
  sawwave
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "WaveGenerator.h"

class SawWaveSound : public WaveGeneratorSound {};

class SawWaveVoice : public WaveGeneratorVoice {

public:
	SawWaveVoice() : level(0), modulo(0) {}

	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override {

		noteFrequency = noteHz(midiNoteNumber, pitchBendCents());
		originalNoteFreq = noteFrequency;
		adsr.noteOn();
		level = velocity * 0.25;
		modulo = 0.0;

		inc = noteFrequency / getSampleRate();
	}



	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
	{
		adsr.setSampleRate(getSampleRate());
		adsr.setParameters(adsrParameters);
		processBlock(outputBuffer, startSample, numSamples);
	}

private:

	void processBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
	{
		while (--numSamples >= 0)
		{
			if (modulo >= 1.0)
				modulo -= 1.0;

			float Sample = (float)(modulo * 2.0 - 1.0) * level * masterGain;

			for (int i = outputBuffer.getNumChannels(); --i >= 0;)
				outputBuffer.addSample(i, startSample, adsr.getNextSample() * Sample);

			modulo += inc;
			++startSample;
		}
	}

	double level, modulo;

};