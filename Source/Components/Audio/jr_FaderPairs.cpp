
#include "jr_FaderPairs.h"
#include <JuceHeader.h>
#include <vector>
#include "jr_Oscillators.h"

//=========================================//
//************ FaderPair ******************//
//=========================================//

void FaderPairs::FaderPair::init(float _sampleRate, bool _silenced)
{
	silenced = _silenced;

	masterGain.reset(_sampleRate, parent.rampTime);
	masterGain.setCurrentAndTargetValue(0.0f);
	masterGain.setTargetValue(silenced ? 0.0f : 1.0f);

	lfo.setSampleRate(_sampleRate);

	lfoBaseFreq = parent.random.nextFloat();
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));

	resetPan(0);
	resetPan(1);

	initOscs(_sampleRate);

	isInitialised = true;
}

void FaderPairs::FaderPair::updateSampleRate(float _sampleRate)
{
	masterGain.reset(_sampleRate, parent.rampTime);

	lfo.setSampleRate(_sampleRate);

	for (auto& osc : oscs)
	{
		osc.setSampleRate(_sampleRate);
	}
}

std::pair<float, float> FaderPairs::FaderPair::process()
{
	if (oscs.size() == 0)
	{
		return out;
	}

	if (masterGain.getCurrentValue() == 0.0f && waitingToRestart)
	{
		waitingToRestart = false;
		start();
	}

	// process next sampleOut
	delta = processLevels(); // avgLevel gets incremented in here

	// reset sample out
	out.first = 0.0f;
	out.second = 0.0f;

	// raw values before panning
	float osc1RawOut = oscs.at(0).process() * (parent.avgLevel.getCurrentValue() + delta);
	float osc2RawOut = oscs.at(1).process() * (parent.avgLevel.getCurrentValue() - delta);

	// add panned signals to out pair
	out.first += osc1RawOut * (1.0f - pan.at(0));
	out.second += osc1RawOut * (pan.at(0));
	out.first += osc2RawOut * (1.0f - pan.at(1));
	out.second += osc2RawOut * (pan.at(1));

	// apply master gain
	masterGain.getNextValue();
	out.first *= masterGain.getCurrentValue();
	out.second *= masterGain.getCurrentValue();

	return out;
}

void FaderPairs::FaderPair::silence()
{
	if (!silenced)
	{
		masterGain.setTargetValue(0);
		silenced = true;
	}
	waitingToRestart = false;
}

void FaderPairs::FaderPair::start()
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

void FaderPairs::FaderPair::updateLfoFreq()
{
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));
}

bool FaderPairs::FaderPair::getIsInitialised()
{
	return isInitialised;
}

void FaderPairs::FaderPair::initOscs(float _sampleRate)
{
	if (oscs.size() != 2)
	{
		for (int i{}; i < 2; i++)
		{
			SineOsc osc = SineOsc();
			osc.setSampleRate(_sampleRate);
			osc.setFrequency(parent.getRandomOscFrequency());
			oscs.push_back(osc);
		}
	}
}

void FaderPairs::FaderPair::resetFrequencies()
{
	for (auto& osc : oscs)
	{
		osc.setFrequency(parent.getRandomOscFrequency());
	}
	lfoBaseFreq = parent.random.nextFloat();
	lfo.setFrequency(getLfoFreqFromScale(lfoBaseFreq));
}

void FaderPairs::FaderPair::resetOsc(int index)
{
	if (index < 0 || index >= oscs.size())
	{
		return;
	}
	oscs.at(index).setFrequency(parent.getRandomOscFrequency());
}

void FaderPairs::FaderPair::resetPan(int index)
{
	if (index < 0 || index >= oscs.size())
	{
		return;
	}
	pan.at(index) = 0.5f + (parent.random.nextFloat() - 0.5f) * parent.stereoWidth; // set to 0.5f +/- 0.5f at max or 0.0f at min
}

float FaderPairs::FaderPair::getLfoFreqFromScale(float scale)
{
	scale = jr::Utils::constrainFloat(scale);
	return parent.minLfoFreq + ((parent.maxLfoFreq * parent.lfoRate) - parent.minLfoFreq) * scale;
}

float FaderPairs::FaderPair::processLevels()
{
	float lfoVal = lfo.process();
	if (lfoVal >= 1.0f)
	{
		resetOsc(1);
		resetPan(1);
	}
	else if (lfoVal <= -1.0f)
	{
		resetOsc(0);
		resetPan(0);
	}
	return lfoVal * parent.avgLevel.getCurrentValue();
}

//=========================================//
//************ FaderPairs *****************//
//=========================================//

void FaderPairs::init(size_t numPairs, float _sampleRate, size_t maxNumPairs)
{
	if (_sampleRate <= 0.0f)
	{
		return;
	}

	sampleRate = _sampleRate;

	gain.reset(sampleRate, 0.1f);
	maxLevel.reset(_sampleRate, rampTime);
	avgLevel.reset(_sampleRate, rampTime);

	if (_pairs.size() == 0)
	{
		// first time only

		setMaxLevel(0.5f / (float)numPairs);

		for (int i{}; i < maxNumPairs; i++)
		{
			_pairs.push_back(FaderPair(*this));
			_pairs.at(i).init(sampleRate, i >= numPairs);
		}

		numActivePairs = numPairs;

		setGainOffset();
	}
	else
	{
		// every time sampleRate changes

		for (auto& pair : _pairs)
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
		for (auto pair : _pairs)
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

	for (auto& pair : _pairs)
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
	avgLevel.setTargetValue(_maxLevel / 2.0f);
}

void FaderPairs::processSharedLevels()
{
	maxLevel.getNextValue();
	avgLevel.getNextValue();
	normalRatio = 1.0f / maxLevel.getCurrentValue();
}

void FaderPairs::setNumPairs(int numPairs)
{
	if (_pairs.size() == 0)
	{
		return;
	}

	if (numPairs == numActivePairs)
	{
		return;
	}
	if (numPairs < 0)
	{
		numPairs = 0;
	}
	else if (numPairs > _pairs.size())
	{
		numPairs = _pairs.size();
	}
	
	setMaxLevel(1.0f / (float)numPairs);

	if (numPairs < numActivePairs) // silencing n pairs
	{
		for (int i{ numActivePairs - 1 }; i >= numPairs; i--)
		{
			_pairs.at(i).silence();
		}
	}
	else if (numPairs > numActivePairs) // starting n pairs
	{
		for (int i{ numActivePairs }; i < numPairs; i++)
		{
			_pairs.at(i).start();
		}
	}

	numActivePairs = numPairs;
	setGainOffset();
}

void FaderPairs::setLfoRate(float _rate)
{
	lfoRate = jr::Utils::constrainFloat(_rate);
	for (auto& pair : _pairs)
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
	auto g1 = 0.3f + 0.2f * jr::Utils::constrainFloat((numActivePairs - 1) / 7.0f); // ramp volume from 0.3 to 0.5 between 1 pair and 7

	gainOffset = jr::Utils::constrainFloat((numActivePairs - 10) / 40.0f); // used to ramp volume from 0.5 to 1.0 between 10 and 40 voices

	gain.setTargetValue(g1 + 0.5f * gainOffset);
}