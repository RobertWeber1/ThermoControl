#pragma once
#include "types.h"
#include "sensor.h"
#include "output.h"

namespace thermo
{

template<class PinInterfece>
struct Channel
{
	LowerTemp lower_bound;
	UpperTemp upper_bound;
	Sensor sensor;
	Output<PinInterfece> output;
};

} //namespace thermo
