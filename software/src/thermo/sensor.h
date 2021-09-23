#pragma once
#include "filter.h"
#include "types.h"


namespace thermo
{


struct Sensor
{
	Sensor(FilterCoefficient coeffitient = FilterCoefficient(0.3f))
	: filter_hotend_(coeffitient)
	, filter_internal_(coeffitient)
	{}

	void process(int16_t first_word, int16_t second_word)
	{
		bool const fault = first_word & 1;
		open_conn_ = second_word & 1;
		short_to_gnd_ = second_word & 2;
		short_to_vcc_ = second_word & 4;

		if(not fault)
		{
			filter_hotend_.value((first_word>>2)/4.0f);
		}
		filter_internal_.value((second_word>>4)/16.0f);
	}

	float hot_end_temperature() const
	{
		return filter_hotend_.value();
	}

	float internal_temperatur() const
	{
		return filter_internal_.value();
	}

	bool has_open_connection() const
	{
		return open_conn_;
	}

	bool has_short_to_gnd() const
	{
		return short_to_gnd_;
	}

	bool has_short_to_vcc() const
	{
		return short_to_vcc_;
	}

	bool has_error() const
	{
		return
			has_open_connection() or
			has_short_to_gnd() or
			has_short_to_vcc();
	}

	char const* error_str()
	{
		if(open_conn_)
		{
			return "Open Connection";
		}

		if(short_to_gnd_)
		{
			return "Short to GND";
		}

		if(short_to_vcc_)
		{
			return "Short to VCC";
		}

		return "No Error";
	}


private:
	Filter filter_hotend_;
	Filter filter_internal_;
	bool open_conn_ = false;
	bool short_to_gnd_ = false;
	bool short_to_vcc_ = false;
};

} //namespace thermo
