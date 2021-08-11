#pragma once
#include "types.h"
#include <chrono>

namespace thermo
{

template<class PinInterfece>
struct Output
{
	using time_point = std::chrono::steady_clock::time_point;

	Output(/*PinInterfece & pin_interface, */Pin pin, MaxCurrent current)
	: pin_(pin)
	, current_(current)
	{}

	time_point last_activation() const
	{
		return last_activation_;
	}

	time_point last_deactivation() const
	{
		return last_deactivation_;
	}

	float current() const
	{
		return value_ ? current_ : 0;
	}

	float requirred_current() const
	{
		return current_;
	}

	bool is_deactivated() const
	{
		return not value_;
	}

	bool is_activated() const
	{
		return value_;
	}

	void activate()
	{
		last_activation_ = std::chrono::steady_clock::now();
		value_ = true;
	}

	void deactivate()
	{
		last_deactivation_ = std::chrono::steady_clock::now();
		value_ = false;
	}

	uint8_t value() const
	{
		return value_ ? 1 : 0;
	}

private:
	Pin pin_;
	MaxCurrent current_;
	time_point last_activation_;
	time_point last_deactivation_;
	bool value_ = false;
};

} //namespace thermo
