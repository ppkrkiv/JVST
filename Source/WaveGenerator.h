/*
  ==============================================================================
  set pitch, mod etc..
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class WaveGeneratorSound : public juce::SynthesiserSound

{
public:
	WaveGeneratorSound() {}//constructor

	bool appliesToNote(int /*midi note nmbr*/) override
	{
		return true;
	}

	bool appliesToChannel(int /*midi channel nmbr*/)  override
	{
		return true;
	}
};

class WaveGeneratorVoice : public juce::SynthesiserVoice
{
public:
	WaveGeneratorVoice() : masterGain(1), inc(0), modWheel(0),
		noteFrequency(0), wheelCoordinate(0)
	{
	}

	bool canPlaySound(juce::SynthesiserSound* sound) override
	{
		return dynamic_cast<WaveGeneratorSound*> (sound) != nullptr;
	}

	//========================================================================================

	void setADSRSampleRate(double sampleRate)
	{
		adsr.setSampleRate(sampleRate);
	}

	void setADSRParameters(float*att,
		float*dec,
		float*sus,
		float*rel)
	{
		adsrParameters.attack = *att;
		adsrParameters.decay = *dec;
		adsrParameters.sustain = *sus;
		adsrParameters.release = *rel;
	}

	//========================================================================================

	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override
	{

	}

	void stopNote(float velocity, bool allowTailOff) override
	{
		adsr.noteOff();
	}

	//========================================================================================

	void pitchWheelMoved(int value)
	{
		setPitchBend(value);
	}

	void roughValuePitchWheelMove(int value) //rough value pass
	{
		wheelCoordinate = value;
	}

	void setBendExtension(int extension)
	{
		pitchBendUpSemitones = extension;
		pitchBendDownSemitones = -extension;
	}

	void recalculatePitch() {
		inc = noteFrequency * std::pow(2.0, pitchBendCents() / 1200) / getSampleRate();
	}

	void setPitchBend(int pitchWheelPos)
	{
		if (pitchWheelPos >= 0)
		{
			// shifting up
			wheelCoordinate = float(pitchWheelPos) / 64;
		}
		else
		{
			// shifting down
			wheelCoordinate = float(pitchWheelPos) / 64;    // negative number
		}
	}

	float pitchBendCents()
	{
		if (wheelCoordinate >= 0.0f)
		{
			// shifting up
			return wheelCoordinate * pitchBendUpSemitones * 100;
		}
		else
		{
			// shifting down
			return -1 * wheelCoordinate * pitchBendDownSemitones * 100;
		}
	}

	//========================================================================================

	static double noteHz(int midiNoteNumber, double centsOffset)
	{
		double hertz = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
		hertz *= std::pow(2.0, centsOffset / 1200);
		return hertz;
	}

	//========================================================================================

	//handles the applied fx with mod wheel position
	void setModWheel(double pos)
	{
		modWheel = pos;
		stretchFrequencies();
	}

	void stretchFrequencies() {
		int sign = 1;
		int fxselection = fxSelected % numFx;

		switch (fxselection)
		{
		case 0:
			sign = -1;
			if (noteFrequency >= originalNoteFreq / 2) {
				noteFrequency *= std::pow(2.0, 64 * modWheel * sign / 1200);
				inc = noteFrequency / getSampleRate();
			}
			break;

		case 1:
			if (noteFrequency > 10000)
				decrescentmod = true;
			else if (noteFrequency < 40)
				decrescentmod = false;
			if (decrescentmod)
				sign = -1;
			noteFrequency *= std::pow(2.0, 64 * modWheel * sign / 1200);
			inc = noteFrequency / getSampleRate();
			break;

		case 2:
			if (noteFrequency > originalNoteFreq + noteFrequency / 40)
				decrescentmod = true;
			else if (noteFrequency < originalNoteFreq - noteFrequency / 40)
				decrescentmod = false;
			if (decrescentmod)
				sign = -1;
			noteFrequency *= std::pow(2.0, 24 * modWheel * sign / 1200);
			inc = noteFrequency / getSampleRate();
			break;

		case 3:

			if (modWheel == 0)
				noteFrequency = originalNoteFreq;
			if (temp > 1)
			{
				temp -= 1;
				noteFrequency = originalNoteFreq + (randomizer.nextFloat() - 0.5) * noteFrequency;
			}
			inc = noteFrequency / getSampleRate();
			temp += 0.2*modWheel;
			break;

		}
	}

	//========================================================================================


	void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override
	{
	}

	//========================================================================================


	void setMasterVolume(double volz) {
		masterGain = volz;
	}

	//========================================================================================

	void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
	{
	}

	void setFx(int val) {
		fxSelected = val;
	}


protected:

	double masterGain;
	double wheelCoordinate;
	double modWheel;
	double inc;
	double temp = 0;

	bool decrescentmod = false;
	int fxSelected = 0;
	int numFx = 4;
	juce::Random randomizer;
	const juce::BigInteger choose = 12;
	juce::ADSR adsr;
	juce::ADSR::Parameters adsrParameters;

	int pitchBendUpSemitones = 2;
	int pitchBendDownSemitones = -2;
	double noteFrequency;
	double originalNoteFreq;
};
