#pragma once
#include "types.h"
#include "sensor.h"
#include "output.h"
#include <chrono>

using namespace std::chrono_literals;

namespace thermo
{

template<class PinInterfece>
struct Channel
{
	LowerTemp lower_bound = LowerTemp(180);
	UpperTemp upper_bound = UpperTemp(200);
	Sensor sensor;
	Output<PinInterfece> output;
};

} //namespace thermo
