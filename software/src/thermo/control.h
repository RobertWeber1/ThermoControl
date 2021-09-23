#pragma once
#include "types.h"
#include "sensor.h"
#include "output.h"
#include "channel.h"
#include <algorithm>
#include <vector>
#include <chrono>
#include <array>
#include <tuple>

namespace thermo
{

using namespace std::chrono_literals;

template<
	class Derived,
	class PinInterfece,
	class StorageInterface,
	uint8_t ChannelCount = 3,
	int LED_PIN = 2,
	int SELECT_PIN = 16>
struct Control
{
	using Channel_t = Channel<PinInterfece>;
	using InputTpye = std::array<Channel_t, ChannelCount>;

	struct Config
	{
		bool is_automatic;
		std::array<
			std::tuple<
				LowerTemp,
				UpperTemp,
				std::chrono::seconds>, ChannelCount> bounds;
	};


	Control(InputTpye const& channels, MaxCurrent max_current)
	: max_current_(max_current)
	, channels_(channels)
	, blink_deadline_(std::chrono::steady_clock::now())
	{
		PinInterfece::make_output(Pin(SELECT_PIN));
		PinInterfece::make_output(Pin(LED_PIN));

		auto cfg =
			StorageInterface::read(
				"config.bin",
				Config{
					false,
					{
						std::make_tuple(
							channels_[0].lower_bound,
							channels_[0].upper_bound,
							std::chrono::seconds(1)),
						std::make_tuple(
							channels_[1].lower_bound,
							channels_[1].upper_bound,
							std::chrono::seconds(1)),
						std::make_tuple(
							channels_[2].lower_bound,
							channels_[2].upper_bound,
							std::chrono::seconds(1))}});

		print(cfg);

		for(size_t i = 0; i < cfg.bounds.size(); ++i)
		{
			channels_[i].lower_bound = std::get<0>(cfg.bounds[i]);
			channels_[i].upper_bound = std::get<1>(cfg.bounds[i]);
			channels_[i].output.set_max_on_time(std::get<2>(cfg.bounds[i]));
		}

		if(cfg.is_automatic)
		{
			set_automatic();
		}
		else
		{
			set_manual();
		}
	}

	std::array<
			std::tuple<
				LowerTemp,
				UpperTemp,
				std::chrono::seconds>, ChannelCount> get_bounds() const
	{
		std::array<
			std::tuple<
				LowerTemp,
				UpperTemp,
				std::chrono::seconds>, ChannelCount> result;

		for(size_t i = 0; i < result.size(); ++i)
		{
			std::get<0>(result[i]) = channels_[i].lower_bound;
			std::get<1>(result[i]) = channels_[i].upper_bound;
			std::get<2>(result[i]) = channels_[i].output.get_max_on_time();
		}

		return result;
	}

	void print(Config const& cfg) const
	{
		printf("Config: %d\n", cfg.is_automatic);

		for(size_t i = 0; i < cfg.bounds.size(); ++i)
		{
			Serial.print(i);
			Serial.print(": ");
			Serial.print("lower: ");
			Serial.print(std::get<0>(cfg.bounds[i]));
			Serial.print(", upper: ");
			Serial.print(std::get<1>(cfg.bounds[i]));
			Serial.print(", on_time: ");
			Serial.println(std::get<2>(cfg.bounds[i]).count());
		}
	}

	void update_config()
	{
		Serial.println("update_config");
		auto const cfg = Config{automatic, get_bounds()};
		StorageInterface::write("config.bin", cfg);
		print(cfg);
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

			// uint8_t channel_counter = 0;
			// for(auto & channel : channels_)
			// {
			// 	Serial.print("Outputs: ");
			// 	Serial.println(output_value, HEX);

			// 	Serial.print("sensor[");
			// 	Serial.print(channel_counter++);
			// 	Serial.print("]: ");
			// 	if(channel.sensor.has_open_connection())
			// 	{
			// 		Serial.println("open connection");
			// 	}
			// 	else if(channel.sensor.has_short_to_gnd())
			// 	{
			// 		Serial.println("short to GND");
			// 	}
			// 	else if(channel.sensor.has_short_to_vcc())
			// 	{
			// 		Serial.println("short to vcc");
			// 	}
			// 	else
			// 	{
			// 		Serial.print("temp: ");
			// 		Serial.print(channel.sensor.hot_end_temperature());
			// 		Serial.print("°C, (");
			// 		Serial.print(channel.sensor.internal_temperatur());
			// 		Serial.println("°C)");
			// 	}
			// }
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
		set_automatic_();
		update_config();
	}

	void set_manual()
	{
		set_manual_();
		update_config();
	}

	void set_bound(size_t id, UpperTemp value)
	{
		if(id >= channels_.size())
		{
			return;
		}

		channels_[id].upper_bound = value;
		update_config();
	}

	void set_bound(size_t id, LowerTemp value)
	{
		if(id >= channels_.size())
		{
			return;
		}

		channels_[id].lower_bound = value;
		update_config();
	}

	void set_max_on_time(size_t id, std::chrono::seconds value)
	{
		if(id >= channels_.size())
		{
			return;
		}

		channels_[id].output.set_max_on_time(value);
		update_config();
	}

	void switch_channel_on(size_t id)
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

	void switch_channel_off(size_t id)
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
		for(size_t i = 0; i < channels_.size(); ++i)
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

	void set_automatic_()
	{
		automatic = true;
	}

	void set_manual_()
	{
		automatic = false;
		for(auto & channel : channels_)
		{
			channel.output.deactivate();
		}
		actual_current_ = 0.0f;
	}

private:
	MaxCurrent max_current_;
	InputTpye channels_;
	std::chrono::steady_clock::time_point blink_deadline_;
	float actual_current_ = 0.0f;
	uint16_t output_counter_ = 1;
	bool automatic = false;
};


} //namespace thermo
