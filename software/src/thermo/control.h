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


template<uint8_t ChannelCount>
struct IOBuffer
{
	uint16_t send[2*(ChannelCount)]; //= {0x434D, 0x0003, 0x434D, 0x0002, 0x434D, 0x0001, 0x434D, 0x0000};
	uint16_t recv[2*(ChannelCount)]; //= {0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};

	static uint8_t units()
	{
		return 2 * (ChannelCount);
	}

	IOBuffer()
	{
		for(uint8_t i = 0; i < (ChannelCount)*2; i=i+2)
		{
			send[i] = 0x434D;
			send[i+1] = i/2;
			recv[i] = 0xffff;
			recv[i+1] = 0xffff;
		}
	}

	void set_channel(uint8_t channel, uint8_t value)
	{
		size_t index = (channel) * 2 + 1;
		if(index < sizeof(send))
		{
			send[index] = (send[index] & 0x00ff) | (uint16_t(value) << 8);
		}
	}

	void print_send() const
	{
		Serial.print("send: ");
		for(size_t i=0; i<ChannelCount; ++i)
		{
			Serial.print(send[i*2], HEX);
			Serial.print(" ");
			Serial.print(send[i*2+1], HEX);
			Serial.print(" ");
		}
		Serial.println("");
	}

	void print_recv() const
	{
		Serial.print("recv: ");
		for(size_t i=0; i<ChannelCount; ++i)
		{
			Serial.print(recv[i*2], HEX);
			Serial.print(" ");
			Serial.print(recv[i*2+1], HEX);
			Serial.print(" ");
		}
		Serial.println("");
	}
};


template<
	class Derived,
	class PinInterfece,
	class StorageInterface,
	uint8_t ChannelCount,
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
	, process_deadline_(std::chrono::steady_clock::now())
	{
		PinInterfece::make_output(Pin(SELECT_PIN));
		PinInterfece::make_output(Pin(LED_PIN));

		PinInterfece::pull_high(Pin(SELECT_PIN));
		PinInterfece::pull_high(Pin(LED_PIN));

		auto cfg =
		// Config{
		// 			true,
		// 			{
		// 				std::make_tuple(
		// 					channels_[0].lower_bound,
		// 					channels_[0].upper_bound,
		// 					60s*2),
		// 				std::make_tuple(
		// 					channels_[1].lower_bound,
		// 					channels_[1].upper_bound,
		// 					60s*2),
		// 				std::make_tuple(
		// 					channels_[2].lower_bound,
		// 					channels_[2].upper_bound,
		// 					60s*2)}};
			StorageInterface::read(
				"/config.bin",
				Config{
					true,
					{
						std::make_tuple(
							channels_[0].lower_bound,
							channels_[0].upper_bound,
							10s),
						std::make_tuple(
							channels_[1].lower_bound,
							channels_[1].upper_bound,
							10s),
						std::make_tuple(
							channels_[2].lower_bound,
							channels_[2].upper_bound,
							10s)}});

		print(cfg);

		for(size_t i = 0; i < cfg.bounds.size(); ++i)
		{
			channels_[i].lower_bound = std::get<0>(cfg.bounds[i]);
			channels_[i].upper_bound = std::get<1>(cfg.bounds[i]);
			channels_[i].output.set_max_on_time(std::get<2>(cfg.bounds[i]));
		}

		if(cfg.is_automatic)
		{
			set_automatic_();
		}
		else
		{
			set_manual_();
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
		Serial.print("\tis_automatic: ");
		Serial.println(cfg.is_automatic);

		for(size_t i = 0; i < cfg.bounds.size(); ++i)
		{
			Serial.print(i);
			Serial.print(": lower: ");
			Serial.print(std::get<0>(cfg.bounds[i]).get());
			Serial.print(", upper: ");
			Serial.print(std::get<1>(cfg.bounds[i]).get());
			Serial.print(", on_time: ");
			Serial.println(std::get<2>(cfg.bounds[i]).count());
		}
	}

	void update_config()
	{
		PinInterfece::pull_high(Pin(SELECT_PIN));
		PinInterfece::pull_high(Pin(LED_PIN));

		Serial.println("update_config");
		auto const cfg = Config{automatic, get_bounds()};
		StorageInterface::write("/config.bin", cfg);

		auto const next_cfg = StorageInterface::template read<Config>("/config.bin");
		print(next_cfg);
	}

	bool is_valid(uint16_t data) const
	{
		if(data == 0 or data == 0xffff or data == 0x8a51)
		{
			return false;
		}

		return true;
	}

	bool buffer_invalid() const
	{
		for(uint8_t i = 0; i < io_buffer.units(); i = i + 2)
		{
			if(not is_valid(io_buffer.recv[i]))
			{
				return true;
			}
		}

		return false;
	}

	void process()
	{

		if(not initialized_)
		{
			PinInterfece::pull_low(Pin(SELECT_PIN));
			PinInterfece::pull_low(Pin(2));
			delay(100);
			Serial.print("sync spi chain");
			while(PinInterfece::transfer(0x8a51) != 0x8a51)
			{
				delay(10);
				Serial.print(".");
			}
			Serial.println("");

			PinInterfece::pull_high(Pin(SELECT_PIN));
			PinInterfece::pull_high(Pin(2));
			initialized_ = true;
		}

		auto const now = std::chrono::steady_clock::now();

		if(now > process_deadline_)
		{
			//Serial.println("process");

			for(size_t index = 0; index < channels_.size(); ++index)
			{
				io_buffer.set_channel(index, channels_[index].output.value());
			}


			do
			{
				//io_buffer.print_send();
				PinInterfece::pull_low(Pin(SELECT_PIN));
				PinInterfece::pull_low(Pin(2));
				delay(10);
				for(uint8_t i = 0; i < io_buffer.units(); i = i + 2)
				{
					io_buffer.recv[i] = PinInterfece::transfer(io_buffer.send[i]);
					io_buffer.recv[i+1] = PinInterfece::transfer(io_buffer.send[i+1]);
					delay(50);

					// Serial.print(i);
					// Serial.print(": ");
					// Serial.print(io_buffer.recv[i], HEX);
					// Serial.print(", ");
					// Serial.println(io_buffer.recv[i+1], HEX);
				}
				PinInterfece::pull_high(Pin(SELECT_PIN));
				PinInterfece::pull_high(Pin(2));
				delay(10);
			}
			while(buffer_invalid());

			// io_buffer.print_recv();

			for(uint8_t i = 0; i < io_buffer.units(); i = i + 2)
			{
				if(is_valid(io_buffer.recv[i]) and is_valid(io_buffer.recv[i+1]))
				{
					channels_[i/2].sensor.process(
						io_buffer.recv[i],
						io_buffer.recv[i+1]);
				}
			}
			process_deadline_ = now + 500ms;
		}

		if(std::chrono::steady_clock::now() >= blink_deadline_)
		{
			blink_deadline_ = std::chrono::steady_clock::now() + 2s;
			PinInterfece::toggle(Pin(2));

			uint8_t channel_counter = 0;
			// for(auto & channel : channels_)
			// {
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
	IOBuffer<ChannelCount> io_buffer;
	std::chrono::steady_clock::time_point blink_deadline_;
	std::chrono::steady_clock::time_point process_deadline_;
	float actual_current_ = 0.0f;
	uint16_t output_counter_ = 1;
	bool automatic = false;
	bool initialized_ = false;
};


} //namespace thermo
