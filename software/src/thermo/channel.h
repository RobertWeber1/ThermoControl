#pragma once
#include "types.h"
#include "sensor.h"
#include "output.h"
#include <chrono>

using namespace std::chrono_literals;

namespace thermo
{

// enum class ChannelState
// {
// 	StartUp,
// 	HysteresisMode
// };

template<class PinInterfece>
struct Channel
{
	LowerTemp lower_bound;
	UpperTemp upper_bound;
	Sensor sensor;
	Output<PinInterfece> output;
	// ChannelState state = ChannelState::StartUp;
};

} //namespace thermo
