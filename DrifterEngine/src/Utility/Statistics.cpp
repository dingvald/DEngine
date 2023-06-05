#include "pch.h"
#include "Statistics.h"

drft::util::Statistics::Statistics(unsigned int maxNumSamples)
	: _maxNumSamples(maxNumSamples)
{}

void drft::util::Statistics::sample(double val)
{
	++_count;
	_max = std::max(_max, val);
	_min = std::min(_min, val);
	_average = (_average * (_maxNumSamples - 1) / _maxNumSamples) + (val / _maxNumSamples);
	if (_count > _maxNumSamples)
	{
		reset();
	}
}

double drft::util::Statistics::average() const
{
	return _average;
}

double drft::util::Statistics::max() const
{
	return _max;
}

double drft::util::Statistics::min() const
{
	return _min;
}

void drft::util::Statistics::reset()
{
	_count = 0;
	_min = std::numeric_limits<double>::max();
	_max = std::numeric_limits<double>::min();
}
