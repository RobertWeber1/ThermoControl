#pragma once
#include "types.h"
#include <chrono>

namespace thermo
{

using namespace std::chrono_literals;

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
		max_on_deadline_ = last_activation_ + max_on_time_;
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

	bool is_max_on() const
	{
		return value_ and std::chrono::steady_clock::now() > max_on_deadline_;
	}

	void set_max_on_time(std::chrono::seconds value)
	{
		max_on_time_ = value;
	}

	std::chrono::seconds get_max_on_time() const
	{
		return max_on_time_;
	}

private:
	Pin pin_;
	MaxCurrent current_;
	time_point last_activation_;
	time_point last_deactivation_;
	time_point max_on_deadline_;
	bool value_ = false;
	std::chrono::seconds max_on_time_ = 60s * 5;
};

} //namespace thermo
