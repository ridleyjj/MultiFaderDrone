
#include "jr_FaderPairs.h"
#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"

//=========================================//
//************ FaderPair ******************//
//=========================================//

void FaderPairs::RandomOsc::init(float _sampleRate, bool _silenced)
{
	silenced = _silenced;

	masterGain.reset(_sampleRate, parent.rampTime);
	masterGain.setCurrentAndTargetValue(0.0f);
	masterGain.setTargetValue(silenced ? 0.0f : 1.0f);

	lfo.setSampleRate(_sampleRate);

	lfoBaseFreq = parent.random.nextFloat();
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));

	resetPan();

	initOsc(_sampleRate);

	isInitialised = true;
}

void FaderPairs::RandomOsc::updateSampleRate(float _sampleRate)
{
	masterGain.reset(_sampleRate, parent.rampTime);

	lfo.setSampleRate(_sampleRate);

	osc.setSampleRate(_sampleRate);
}

std::pair<float, float> FaderPairs::RandomOsc::process()
{
	if (masterGain.getCurrentValue() == 0.0f && waitingToRestart)
	{
		waitingToRestart = false;
		start();
	}

	// process next sampleOut
	currentLevel = processLevel();

	// reset sample out
	out.first = 0.0f;
	out.second = 0.0f;

	// raw value before panning
	float oscRawOut = osc.process() * currentLevel * masterGain.getNextValue();

	// add panned signals to out pair
	out.first = oscRawOut * (1.0f - pan);
	out.second = oscRawOut * pan;

	return out;
}

void FaderPairs::RandomOsc::silence()
{
	if (!silenced)
	{
		masterGain.setTargetValue(0);
		silenced = true;
	}
	waitingToRestart = false;
}

void FaderPairs::RandomOsc::start()
{
	if (masterGain.getCurrentValue() != 0.0f && silenced)
	{
		// if the voice is silenced but has not yet been faded completely, wait
		waitingToRestart = true;
	}
	else if (masterGain.getCurrentValue() == 0.0f)
	{
		// if voice is already silenced reset it and start
		resetFrequencies();
		masterGain.setTargetValue(1.0f);
		silenced = false;
	}
	// if voice is not currently silenced do nothing
}

void FaderPairs::RandomOsc::updateLfoFreq()
{
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));
}

bool FaderPairs::RandomOsc::getIsInitialised()
{
	return isInitialised;
}

void FaderPairs::RandomOsc::initOsc(float _sampleRate)
{
	if (!isInitialised)
	{
		osc = SineOsc();
		osc.setSampleRate(_sampleRate);
		osc.setFrequency(parent.getRandomOscFrequency());
	}
}

void FaderPairs::RandomOsc::resetFrequencies()
{
	osc.setFrequency(parent.getRandomOscFrequency());
	lfoBaseFreq = parent.random.nextFloat();
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));
}

void FaderPairs::RandomOsc::resetOsc()
{
	osc.setFrequency(parent.getRandomOscFrequency());
}

void FaderPairs::RandomOsc::resetPan()
{
	pan = 0.5f + (parent.random.nextFloat() - 0.5f) * parent.stereoWidth; // set to 0.5f +/- 0.5f at max or 0.0f at min
}

float FaderPairs::RandomOsc::getLfoFreqFromScale(float scale)
{
	scale = jr::Utils::constrainFloat(scale);
	return parent.minLfoFreq + ((parent.maxLfoFreq * parent.lfoRate) - parent.minLfoFreq) * scale;
}

float FaderPairs::RandomOsc::processLevel()
{
	float lfoVal = lfo.process();
	lfoVal += 1.0f; // between 0-2
	lfoVal /= 2.0f; // between 0-1
	if (lfoVal <= 0.001f)
	{
		resetOsc();
	}
	return lfoVal * parent.avgLevel.getCurrentValue();
}

//=========================================//
//************ FaderPairs *****************//
//=========================================//

void FaderPairs::init(size_t numOscs, float _sampleRate, size_t maxNumOscs)
{
	if (_sampleRate <= 0.0f)
	{
		return;
	}

	sampleRate = _sampleRate;

	gain.reset(sampleRate, 0.1f);
	maxLevel.reset(_sampleRate, rampTime);
	avgLevel.reset(_sampleRate, rampTime);

	if (_oscs.size() == 0)
	{
		// first time only

		setMaxLevel(0.5f / (float)numOscs);

		for (int i{}; i < maxNumOscs; i++)
		{
			_oscs.push_back(RandomOsc(*this));
			_oscs.at(i).init(sampleRate, i >= numOscs);
		}

		numActiveOscs = numOscs;

		setGainOffset();
	}
	else
	{
		// every time sampleRate changes

		for (auto& pair : _oscs)
		{
			pair.updateSampleRate(sampleRate);
		}
	}
}

std::pair<float, float> FaderPairs::process()
{
	out.first = 0.0f;
	out.second = 0.0f;

	if (!isInitialised)
	{
		bool allFinished = true;
		for (auto pair : _oscs)
		{
			if (!pair.getIsInitialised())
			{
				allFinished = false;
				break;
			}
		}
		isInitialised = allFinished;
		if (!isInitialised)
		{
			return out;
		}
	}

	float sampleOut{};

	processSharedLevels();

	for (auto& pair : _oscs)
	{
		auto pairOut = pair.process();
		out.first += pairOut.first;
		out.second += pairOut.second;
	}

	gain.getNextValue();

	out.first *= gain.getCurrentValue();
	out.second *= gain.getCurrentValue();

	return out;
}

void FaderPairs::setMaxLevel(float _maxLevel)
{
	maxLevel.setTargetValue(_maxLevel);
}

void FaderPairs::processSharedLevels()
{
	maxLevel.getNextValue();
	normalRatio = 1.0f / maxLevel.getCurrentValue();
}

void FaderPairs::setNumOscs(int numOscs)
{
	if (_oscs.size() == 0)
	{
		return;
	}

	if (numOscs == numActiveOscs)
	{
		return;
	}
	if (numOscs < 0)
	{
		numOscs = 0;
	}
	else if (numOscs > _oscs.size())
	{
		numOscs = _oscs.size();
	}
	
	setMaxLevel(1.0f / (float)numOscs);

	if (numOscs < numActiveOscs) // silencing n oscs
	{
		for (int i{ numActiveOscs - 1 }; i >= numOscs; i--)
		{
			_oscs.at(i).silence();
		}
	}
	else if (numOscs > numActiveOscs) // starting n oscs
	{
		for (int i{ numActiveOscs }; i < numOscs; i++)
		{
			_oscs.at(i).start();
		}
	}

	numActiveOscs = numOscs;
	setGainOffset();
}

void FaderPairs::setLfoRate(float _rate)
{
	lfoRate = jr::Utils::constrainFloat(_rate);
	for (auto& pair : _oscs)
	{
		pair.updateLfoFreq();
	}
}

void FaderPairs::setStereoWidth(float width)
{
	stereoWidth = jr::Utils::constrainFloat(width);
}

void FaderPairs::setGainOffset()
{
	auto g1 = 0.3f + 0.2f * jr::Utils::constrainFloat((numActiveOscs - 1) / 14.0f); // ramp volume from 0.3 to 0.5 between 1 osc and 14

	gainOffset = jr::Utils::constrainFloat((numActiveOscs - 20) / 80.0f); // used to ramp volume from 0.5 to 1.0 between 20 and 80 oscs

	gain.setTargetValue(g1 + 0.5f * gainOffset);
}