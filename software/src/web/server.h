#pragma once
#include <ArduinoWebsockets.h>
#include <stdint.h>
#include <chrono>
#include <array>
#include "index.h"


namespace web
{

using namespace std::chrono_literals;

template<
	class Derived,
	uint8_t WSClientCount = 2,
	uint16_t HttpPort = 80,
	uint16_t WsPort = 8080>
struct Server
{
	using milliseconds = std::chrono::milliseconds;
	using time_point = std::chrono::steady_clock::time_point;

	Server()
	{
		Serial.println(WiFi.softAP("AndroidAP1", "androidap") ? "Ready" : "Failed!");

		Serial.print("AP IP address: ");
		Serial.println(WiFi.softAPIP());
		Serial.println("WebServer Started");

		ws_server.listen(WsPort);
		web_server.begin();
	}

	void process()
	{
		if(seconds_timer <= std::chrono::steady_clock::now())
		{
			seconds_timer = std::chrono::steady_clock::now() + milliseconds(1000);
			derived().tick();
		}
		process_web_server();
		process_ws_server();
		process_ws_clients();
	}

	void publish(String const& data)
	{
		for(auto & client : ws_clients)
		{
			client.send(data);
		}
	}

private:
	void process_web_server()
	{
		auto client = web_server.available();
		if(client)
		{
			Serial.println("deliver web page");
			client.println(main_page);
		}
	}

	void process_ws_server()
	{
		if(not ws_server.poll())
		{
			return;
		}

		int slot = get_ws_slot();
		if(slot == -1)
		{
			return;
		}

		Serial.println("ws_server.acccept");
		ws_clients[slot].init(slot, derived(), ws_server.accept());
		Serial.print("[");
		Serial.print(slot);
		Serial.println("] WS-Client connected");

		derived().client_connected(ws_clients[slot]);
	}

	void process_ws_clients()
	{
		for(auto & client : ws_clients)
		{
			client.process();
		}
	}

	int get_ws_slot()
	{
		for(int i= 0; i<WSClientCount; ++i)
		{
			if(not ws_clients[i].available())
			{
				return i;
			}
		}

		return -1;
	}

private:
	WiFiServer web_server = WiFiServer(HttpPort);
	websockets::WebsocketsServer ws_server;

public:
	struct WSClient : websockets::WebsocketsClient
	{
		void print(const char* s)
		{
			Serial.print("[");
			Serial.print(slot_);
			Serial.print("] ");
			Serial.println(s);
		}

		template<class Context>
		void init(int slot, Context & context, const websockets::WebsocketsClient& other)
		{
			slot_ = slot;
			static_cast<websockets::WebsocketsClient&>(*this) = other;
			using namespace websockets;

			onEvent(
				[&, &context, this](WebsocketsClient& c, WebsocketsEvent type, WSInterfaceString)
				{
					switch(type)
					{
					case WebsocketsEvent::ConnectionOpened:

						print("connected   !!!!!!!!!!!!!!!!!!!");
						context.client_connected(*this);
						break;

					case WebsocketsEvent::ConnectionClosed:
						print("disconnected");
						heartbeat_deadline = time_point::max();
						heartbeat_response_deadline = time_point::max();
						break;

					case WebsocketsEvent::GotPing:
						print("ping");
						break;

					case WebsocketsEvent::GotPong:
						print("pong");
						break;
					}
				});

			onMessage(
				[&, &context](WebsocketsClient& client, WebsocketsMessage const& msg)
				{
					if(msg.data() == "---Heatbeat---")
					{
						//print("received ---Heatbeat---");
						heartbeat_response_deadline = time_point::max();
						heartbeat_deadline =
							hb_request_interval +
							std::chrono::steady_clock::now();
					}
					else
					{
						context.received_message(msg.data());
					}
				});

			heartbeat_deadline = std::chrono::steady_clock::now() + 2s;
			heartbeat_response_deadline = time_point::max();
		}

		void process()
		{
			if(available())
			{
				poll();
				if(heartbeat_deadline <= std::chrono::steady_clock::now())
				{
					//print("xxxxxxxxxx send heartbeat");
					if(send("---Heatbeat---"))
					{
						heartbeat_response_deadline =
							std::chrono::steady_clock::now() + hb_response_interval;
						heartbeat_deadline = time_point::max();
						print("\t\theartbeat_response_deadline resetted");
					}
					else
					{
						print("\t\tsend heartbeat failed");
						close();
						heartbeat_response_deadline = time_point::max();
						heartbeat_deadline = time_point::max();
					}
				}
				else if(heartbeat_response_deadline <= std::chrono::steady_clock::now())
				{
					print("heartbeat timed out!");
					close();
					heartbeat_deadline = time_point::max();
					time_point heartbeat_deadline = time_point::max();
				}
			}
		}

	private:
		time_point heartbeat_deadline = std::chrono::steady_clock::now();
		time_point heartbeat_response_deadline = time_point::max();

		int slot_ = 0;
		const milliseconds hb_request_interval = milliseconds(10000);
		const milliseconds hb_response_interval = milliseconds(5000);
	};

private:
	Derived& derived()
	{
		return static_cast<Derived&>(*this);
	}

private:
	std::array<WSClient, WSClientCount> ws_clients;

	time_point seconds_timer = std::chrono::steady_clock::now();
};

} //namespace web
