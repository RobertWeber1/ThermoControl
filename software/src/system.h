#pragma once
#include "thermo/control.h"
#include "thermo/spi_io.h"
#include "web/server.h"
#include "web/json_stream.h"
#include <chrono>

template<class PinInterfece, class SpiInterface>
struct System
: web::Server<System<PinInterfece, SpiInterface>>
, thermo::Control<System<PinInterfece, SpiInterface>, PinInterfece>
{
	using Self_t = System<PinInterfece, SpiInterface>;
	using Server_t = web::Server<Self_t>;
	using Control_t = thermo::Control<Self_t, PinInterfece>;
	using time_point = std::chrono::steady_clock::time_point;
	using Channel_t = typename Control_t::Channel_t;
	using Control_t::Control;

	void process()
	{
		Control_t::process();
		Server_t::process();
	}

	void client_connected(/*Server_t::WSClient & client*/)
	{
		update_gui();
	}

	void received_message(String msg)
	{
		Serial.println("System received message");
		Serial.println(msg.c_str());
		web::JsonStream stream(msg.c_str(), msg.length());

		std::string type;
		stream >> type;

		Serial.print("type: ");
		Serial.println(type.c_str());

		//{set_mode: "AUTO"/"MANUAL}"
		if(type == "set_mode")
		{
			std::string mode;
			stream >> mode;

			if(mode == "AUTO")
			{
				Control_t::set_automatic();
			}
			else
			{
				Control_t::set_manual();
			}
		}
		//{set_limit: [channelId, "UPPER"/"LOWER", 123]}
		else if(type == "set_limit")
		{
			std::string id;
			std::string bounds;
			std::string value;
			stream >> id >> bounds >> value;

			Serial.print("id: ");
			Serial.println(id.c_str());
			Serial.print("bound: ");
			Serial.println(bounds.c_str());
			Serial.print("value: ");
			Serial.println(value.c_str());
			if(bounds == "UPPER")
			{
				Control_t::set_bound(atoi(id.c_str()), thermo::UpperTemp(atoi(value.c_str())));
			}
			else
			{
				Control_t::set_bound(atoi(id.c_str()), thermo::LowerTemp(atoi(value.c_str())));
			}
		}
		//{switch: [channelId, "ON"/"OFF"]}
		else if(type == "switch")
		{
			std::string id;
			std::string value;
			stream >> id >> value;

			Serial.print("id: ");
			Serial.println(id.c_str());
			Serial.print("value: ");
			Serial.println(value.c_str());


			if(value == "ON")
			{
				Control_t::switch_channel_on(atoi(id.c_str()));
			}
			else
			{
				Serial.println("switch channel OFF!!!!!");
				Control_t::switch_channel_off(atoi(id.c_str()));
			}
		}
	}

	void tick()
	{
		update_gui();
	}

private:
	static std::string enquote(std::string const& s)
	{
		return "\"" + s + "\"";
	}

	static std::string enquote(int const& i)
	{
		return "\"" + std::to_string(i) + "\"";
	}

	static std::string error_str(
		bool open_connection,
		bool short_to_gnd,
		bool short_to_vcc)
	{
		if(open_connection)
		{
			return "Open Connection";
		}

		if(short_to_gnd)
		{
			return "Short to GND";
		}

		if(short_to_vcc)
		{
			return "Short to VCC";
		}

		return "OK";
	}

	static std::string str(int i)
	{
		return std::to_string(i);
	}

	static std::string str(bool b)
	{
		return b ? "true" : "false";
	}

	static std::string str(float i)
	{
		return std::to_string(i);
	}

	void update_gui()
	{
		std::string data = "{"
			+ enquote("automaitc_mode") + ":" + str(Control_t::is_automatic()) + ","
			+ enquote("max_current") + ":" + str(Control_t::max_current()) + ","
			+ enquote("actual_current") + ":" + str(Control_t::actual_current()) + ","
			+ enquote("channels") + ":{";
		Control_t::for_each_channel(
			[&data](int id, int count, Channel_t const& channel)
			{
				data +=
					enquote(id) + ":{" +
					enquote("lower_bound") + ":" + str(channel.lower_bound.get()) + "," +
					enquote("upper_bound") + ":" + str(channel.upper_bound.get()) + "," +
					enquote("temperature") + ":" + str(channel.sensor.hot_end_temperature()) + "," +
					enquote("max_current") + ":" + str(channel.output.current()) + "," +
					enquote("sensor_status") + ":" +
						enquote(error_str(
							channel.sensor.has_open_connection(),
							channel.sensor.has_short_to_gnd(),
							channel.sensor.has_short_to_vcc())) + "," +
					enquote("output") + ":"	+ str(channel.output.is_activated()) + "}";

					if(id < count - 1)
					{
						data += ",";
					}
			});

		data += "}}";

		// Serial.println("send update message: ");
		// Serial.println(data.c_str());

		Server_t::publish(data.c_str());
	}

private:
	time_point next_gui_update = std::chrono::steady_clock::now();
};
