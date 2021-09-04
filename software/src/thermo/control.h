#pragma once
#include "types.h"
#include "sensor.h"
#include "output.h"
#include "channel.h"
#include <algorithm>
#include <vector>
#include <chrono>



namespace thermo
{

using namespace std::chrono_literals;

template<
	class Derived,
	class PinInterfece,
	class StorageInterface,
	int LED_PIN = 2,
	int SELECT_PIN = 16>
struct Control
{
	using Channel_t = Channel<PinInterfece>;

	Control(std::vector<Channel_t> const& channels, MaxCurrent max_current)
	: channels_(channels)
	, max_current_(max_current)
	, blink_deadline_(std::chrono::steady_clock::now())
	{
		PinInterfece::make_output(Pin(SELECT_PIN));
		PinInterfece::make_output(Pin(LED_PIN));
	}

	void process()
	{
		uint16_t output_value = 0;

		for(size_t index = 0; index < channels_.size(); ++index)
		{
			output_value |= channels_[index].output.value() << (index+4);
		}

		for(size_t index = 0; index < channels_.size(); ++index)
		{
			PinInterfece::transfer(~((output_value << 8) | 1<<index));

			PinInterfece::pull_high(Pin(SELECT_PIN));
			delay(1);
			PinInterfece::pull_low(Pin(SELECT_PIN));

			uint16_t first = PinInterfece::transfer(0);
			uint16_t second = PinInterfece::transfer(0);

			channels_[index].sensor.process(first, second);
		}

		if(std::chrono::steady_clock::now() >= blink_deadline_)
		{
			blink_deadline_ = std::chrono::steady_clock::now() + 2s;
			PinInterfece::toggle(Pin(2));

			uint8_t channel_counter = 0;
			for(auto & channel : channels_)
			{
				Serial.print("Outputs: ");
				Serial.println(output_value, HEX);

				Serial.print("sensor[");
				Serial.print(channel_counter++);
				Serial.print("]: ");
				if(channel.sensor.has_open_connection())
				{
					Serial.println("open connection");
				}
				else if(channel.sensor.has_short_to_gnd())
				{
					Serial.println("short to GND");
				}
				else if(channel.sensor.has_short_to_vcc())
				{
					Serial.println("short to vcc");
				}
				else
				{
					Serial.print("temp: ");
					Serial.print(channel.sensor.hot_end_temperature());
					Serial.print("°C, (");
					Serial.print(channel.sensor.internal_temperatur());
					Serial.println("°C)");
				}
			}
		}

		if(not automatic)
		{
			return;
		}

		actual_current_ = 0.0f;

		for(auto & channel : channels_)
		{
			if(channel.sensor.hot_end_temperature() > channel.upper_bound or
			   channel.sensor.has_error() or
			   channel.output.is_max_on())
			{
				channel.output.deactivate();
			}
			actual_current_ += channel.output.current();
		}

		std::vector<Channel_t*> candidates_for_activation;
		for(auto & channel : channels_)
		{
			if((channel.sensor.hot_end_temperature() < channel.lower_bound) and
			   channel.output.is_deactivated())
			{
				candidates_for_activation.push_back(&channel);
			}
		}

		std::sort(
			candidates_for_activation.begin(),
			candidates_for_activation.end(),
			[](auto & lhs, auto & rhs)
			{
				return lhs->output.last_deactivation() < rhs->output.last_deactivation();
			});

		for(auto & channel : candidates_for_activation)
		{

			if(actual_current_ + channel->output.requirred_current() <= max_current_)
			{
				channel->output.activate();
				actual_current_ += channel->output.requirred_current();
			}
		}
	}

	void set_automatic()
	{
		automatic = true;
	}

	void set_manual()
	{
		automatic = false;
		for(auto & channel : channels_)
		{
			channel.output.deactivate();
		}
		actual_current_ = 0.0f;
	}

	void set_bound(int id, UpperTemp value)
	{
		if(id >= channels_.size())
		{
			return;
		}

		channels_[id].upper_bound = value;
	}

	void set_bound(int id, LowerTemp value)
	{
		if(id >= channels_.size())
		{
			return;
		}

		channels_[id].lower_bound = value;
	}

	void set_max_on_time(int id, std::chrono::seconds value)
	{
		if(id >= channels_.size())
		{
			return;
		}

		channels_[id].output.set_max_on_time(value);
	}

	void switch_channel_on(int id)
	{
		if(automatic)
		{
			Serial.println("not in manual mode");
			return;
		}

		if(id >= channels_.size())
		{
			Serial.println("channel id invalid");
			return;
		}

		auto & channel = channels_[id];

		if(channel.output.is_activated())
		{
			Serial.println("ouput alreay active");
			return;
		}

		if(actual_current_ + channel.output.requirred_current() > max_current_)
		{
			Serial.println("no current avalilable");
			return;
		}

		channel.output.activate();
		actual_current_ += channel.output.requirred_current();
	}

	void switch_channel_off(int id)
	{
		if(automatic)
		{
			Serial.println("not in manual mode");
			return;
		}

		if(id >= channels_.size())
		{
			Serial.println("channel id invalid");
			return;
		}

		auto & channel = channels_[id];

		if(channel.output.is_deactivated())
		{
			Serial.println("output alreay inactive");
			return;
		}

		Serial.println("deactivae channel");
		channel.output.deactivate();
		actual_current_ -= channel.output.requirred_current();
	}

	template<class Func>
	void for_each_channel(Func func) const
	{
		for(int i = 0; i < channels_.size(); ++i)
		{
			func(i, channels_.size(), channels_[i]);
		}
	}

	bool is_automatic() const
	{
		return automatic;
	}

	float max_current() const
	{
		return max_current_.get();
	}

	float actual_current() const
	{
		return actual_current_;
	}

	size_t channel_count() const
	{
		return channels_.size();
	}

private:
	Derived& derived()
	{
		return static_cast<Derived&>(*this);
	}

private:
	MaxCurrent max_current_;
	std::vector<Channel_t> channels_;
	bool automatic = false;
	float actual_current_ = 0.0f;
	std::chrono::steady_clock::time_point blink_deadline_;
	uint16_t output_counter_ = 1;
};


} //namespace thermo
