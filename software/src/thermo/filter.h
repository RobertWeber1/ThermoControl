#pragma once
#include "types.h"

namespace thermo
{

struct Filter
{
	Filter(FilterCoefficient f)
	: factor(f)
	{}

	float value(float next)
	{
		return previous = factor * previous + (1-factor) * next;
	}

	float value() const
	{
		return previous;
	}

private:
	float previous = 0.0f;
	float factor;
};

} //namespace thermo
