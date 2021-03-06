#pragma once
#include <ArduinoWebsockets.h>
#include <stdint.h>
#include <chrono>
#include <array>
// #include <index.h>


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
	char const* main_page = R"XML(
<!DOCTYPE HTML>
<html lang="en">
<head>
	<meta charset="utf-8" />
	<meta name='viewport' content='width=device-width' />
	<title>ThermoControl</title>
	<link rel="shortcut icon" href="data:image/ico;base64,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"/>
	<style>
			body{
			margin: 0;
			background: #EFEFEF;
			font-family: sans-serif;
		}

		#Connecting {
			width: 100%;
			background-color: grey;
			text-align: center;
			color: white;
		}

		#ConnectingContent {
			display: flex;
			justify-content: center;
			align-items: center;
		}

		.Spinner {
			margin-left: 5px;
			border: 6px solid #f3f3f3;
			border-top: 6px solid #3498db;
			border-radius: 50%;
			width: 15px;
			height: 15px;
			animation: spin 4s linear infinite;
		}

		#Connected{
			margin:0;
		}

		#Main{
			display: flex;
			justify-content: center;
			align-items: center;
			flex-direction: column;
		}

		#HeadLine{
			margin: 0;
			border: 0;
			width: 100%;
			background-color: #4AA87E;
			text-align: center;
			color: white;
		}

		@keyframes spin {
			0% { transform: rotate(0deg); }
			100% { transform: rotate(360deg); }
		}

		.switch_div {
			display: flex;
			align-items: center;
				justify-content: center;
		}

		.switch {
		  position: relative;
		  display: inline-block;
		  width: 120px;
		  height: 34px;
		  margin: 5px;
		}

		.switch input {
		  opacity: 0;
		  width: 0;
		  height: 0;
		}

		.slider {
		  position: absolute;
		  cursor: pointer;
		  top: 0;
		  left: 0;
		  right: 0;
		  bottom: 0;
		  background-color: #ccc;
		  -webkit-transition: .4s;
		  transition: .4s;
		}

		.slider:before {
		  position: absolute;
		  content: "";
		  height: 26px;
		  width: 26px;
		  left: 4px;
		  bottom: 4px;
		  background-color: grey;
		  -webkit-transition: .4s;
		  transition: .4s;
		}

		input:checked + .slider {
		  background-color: #ccc;
		}

		input:focus + .slider {
		  box-shadow: 0 0 1px #ccc;
		}

		input:checked + .slider:before {
		  -webkit-transform: translateX(26px);
		  -ms-transform: translateX(26px);
		  transform: translateX(86px);
		}

		.slider.round {
		  border-radius: 34px;
		}

		.slider.round:before {
		  border-radius: 50%;
		}

		.channel {
			width: 100%;
			display: flex;
			flex-wrap: wrap;
			flex-direction: row;
			align-items: center;
			background-color: grey;
			margin-bottom: 10px;
			padding: 10px;
			color: white;
			border-radius: 25px;
		}

		.channel_label {
			width: 70px;
			font-size: 40pt;
			margin-left: 10px;
			font-weight: bold;
		}

		.right {
			margin-left: auto;
			padding-right: 10px;
		}

		.mode_div {
			display: flex;
			align-items: center;
			justify-content: center;
		}

		.large_text{
			font-size: 25pt;
		}

		.FooInput {
			margin: 5px;
		}

	</style>
	<script>
		var connection = null;
		var reconnect_timer = null;

		function open_web_socket()
		{
			console.log("try connect");
			if(reconnect_timer != null)
			{
				clearTimeout(reconnect_timer);
				reconnect_timer = null;
			}

			connection = new WebSocket("ws://192.168.4.1:8080/");

			var heartbeat_interval = null;

			var missed_heartbeats = 0;
			function check_heartbeat()
			{
				clearInterval(heartbeat_interval);
				missed_heartbeats = 0;
			}
			connection.onopen =
				function ()
				{
					missed_heartbeats = 0;
					check_heartbeat();
					clearInterval(heartbeat_interval);
					heartbeat_interval =
						setInterval(function()
							{
								connection.send("---Heatbeat---");
							}, 1000);

					connected_event();
				};
			connection.onmessage =
				function (msg)
				{
					if(msg.data === "---Heatbeat---")
					{
						missed_heartbeats = 0;
						connection.send("---Heatbeat---");
					}
					else
					{
						process_message(msg.data);
					}
				};
			connection.onerror =
				function ()
				{
					console.log("WebSocket closed, try reconnect in 500ms...");
					reconnect_timer = setTimeout(open_web_socket, 50000);
					clearInterval(heartbeat_interval);
				};
			connection.onclose =
				function ()
				{
					console.log("connecton closed by Server?");
					connection.onerror();
				};
		}


		function enable(id)
		{
			document.getElementById(id).disabled = false;
			document.getElementById(id).style.display = "block";
		}

		function disable(id)
		{
			document.getElementById(id).style.display = "none";
		}


		function switch_mode(enable_auto)
		{
			if(enable_auto)
			{
				send_command("set_mode", "AUTO");
			}
			else
			{
				send_command("set_mode", "MANUAL");
			}
		}

		function mode_switched(automatic_mode)
		{
			const on = document.getElementById(key + "OFF");
			const off = document.getElementById(key + "ON");
			const upper = document.getElementById(key + "UPPER");
			const lower = document.getElementById(key + "LOWER");
			const on_time = document.getElementById(key + "ONTIME");

			if(automatic_mode == false)
			{
				if(false)
				{
					on.style = "display:block;";
					off.style = "display:none;";
				}
				else
				{
					on.style = "display:none;";
					off.style = "display:block;";
				}
				upper.style = "display:none;";
				lower.style = "display:none;";
			}
			else
			{
				on.style = "display:none;";
				off.style = "display:none;";

				upper.style = "display:block;";
				lower.style = "display:block;";
			}
		}

		function create_input(id, type, on_change, value)
		{
			const input = document.createElement("input");
			input.id = id;
			input.classList.add("FooInput")
			input.type = type;
			input.step = 10;
			input.onchange = on_change;
			input.value = value;

			return input;
		}

		function create_p_input(name, id, type, on_change, value)
		{
			const para = document.createElement("p");
			para.innerHTML = name;
			const val = document.createElement("a");
			val.id = "val_" + id;
			para.append(val);
			para.append(document.createElement("br"));
			para.append(create_input(id, type, on_change, value));

			return para;
		}

		function create_slider(id, left, right, on_click)
		{
			const input = document.createElement("input");
			input.type = "checkbox";
			input.id = id;
			input.onclick = on_click;

			const span = document.createElement("span");
			span.classList.add("slider");
			span.classList.add("round");

			const label = document.createElement("label");
			label.classList.add("switch");
			label.appendChild(input);
			label.appendChild(span);

			const div = document.createElement("div");
			div.classList.add("switch_div");
			div.append(left);
			div.appendChild(label);
			div.append(right);
			return div;
		}

		function setup(count)
		{
			console.log("setup ", count, " channels");

			var channels = document.getElementById("channels");
			channels.innerHTML = "";

			for(var index = 0; index < count; index = index + 1)
			{
				const channel = document.createElement("div");
				channel.classList.add("channel");
				channel.id = index;

				const identifier = document.createElement("div");
				identifier.innerHTML = index + 1;
				identifier.classList.add("channel_label");

				channel.appendChild(identifier);

				var middle_div = document.createElement("div");
				middle_div.classList.add("left");

				const p_temp = document.createElement("p");
				p_temp.append("Temperatur: ");
				p_temp.appendChild(document.createElement("br"));
				const temp = document.createElement("a");
				temp.classList.add("large_text");
				temp.id = index + "temp";
				temp.innerHTML = "-";
				p_temp.appendChild(temp);
				middle_div.appendChild(p_temp);


				const p_output = document.createElement("p");
				p_output.append("Ausgang: ");
				p_output.appendChild(document.createElement("br"));
				const output = document.createElement("a");
				output.id = index + "output";
				output.innerHTML = "-";
				output.classList.add("large_text");
				p_output.appendChild(output);
				p_output.appendChild(
					create_slider(
						index+"_switch",
						"AUS",
						"AN",
						function(i) {
							console.log("add slider: ", i);
							return function(event) {
								console.log(i, event.target.checked);
								send_command(
									"switch",
									[i, (event.target.checked)?"ON":"OFF"]); };
						}(index)
						));

				middle_div.appendChild(p_output);


				channel.appendChild(middle_div);

				var settings_div = document.createElement("div");
				settings_div.classList.add("right");
				settings_div.appendChild(
					create_p_input(
						"Obergrenze: ",
						index + "UPPER",
						"number",
						send_channel_update,
						0));

				settings_div.appendChild(
					create_p_input(
						"Untergrenze: ",
						index + "LOWER",
						"number",
						send_channel_update,
						0));

				settings_div.appendChild(
					create_p_input(
						"Anzeit: ",
						index + "ONTIME",
						"number",
						send_channel_update,
						120));

				channel.appendChild(settings_div);

				channels.appendChild(channel);
			}
		}

		function process_message(raw_data)
		{
			process_json_message(JSON.parse(raw_data));
		}

		function process_json_message(message)
		{
			//console.log("process_json_message: ", message);
			if(message.type == "setup")
			{
				//console.log(message);
		    	setup(message.data);
			}
			else if(message.type == "data")
			{
				const data = message.data;

				//console.log(data);

				for(const key in data.channels)
				{
					const ch_data = data.channels[key];

					const upper = document.getElementById(key + "UPPER");
					const upper_val = document.getElementById("val_" + key + "UPPER");
					const lower = document.getElementById(key + "LOWER");
					const lower_val = document.getElementById("val_" + key + "LOWER");
					const on_time = document.getElementById(key + "ONTIME");
					const on_time_val = document.getElementById("val_" + key + "ONTIME");
					const switch_button = document.getElementById(key + "_switch");
					const switch_button_div = switch_button.parentNode.parentNode;

					const temp = document.getElementById(key + "temp");
					const output = document.getElementById(key + "output");

					const switch_mode = document.getElementById("ModeSwitch");

					switch_mode.checked = data.automaitc_mode;
					switch_button.checked = ch_data.output;

					if(data.automaitc_mode == false)
					{
						upper.style = "display:none;";
						lower.style = "display:none;";
						on_time.style = "display:none;";
						switch_button_div.style ="display:flex;";
						output.style = "display:none;";
					}
					else
					{
						switch_button_div.style ="display:none;";
						upper.style = "display:block;";
						lower.style = "display:block;";
						on_time.style = "display:block;";
						output.style = "display:block;";
					}

					upper_val.innerHTML = ch_data.upper_bound + " ??C";
					lower_val.innerHTML = ch_data.lower_bound + " ??C";
					on_time_val.innerHTML = ch_data.max_on_time + " s";

					if(ch_data.sensor_status == "OK")
					{
						temp.innerHTML = ch_data.temperature.toFixed(1) + " ??C";
					}
					else
					{
						temp.innerHTML = ch_data.sensor_status;
					}

					if(ch_data.output)
					{
						output.innerHTML = "AN";
					}
					else
					{
						output.innerHTML = "AUS";
					}
				}
			}
		}

		function test_data()
		{
			const data = {
				type: "data",
				data: {
					automaitc_mode: true,
					max_current: 30,
					actual_current: 30,
					channels: {
						"0": {
							lower_bound: 101,
							upper_bound: 201,
							temperature: 35,
							max_current: 30,
							sensor_status: "Short To Ground",
							output: false,
							ontime: 0,
							laston: 234},
						"1": {
							lower_bound: 102,
							upper_bound: 202,
							temperature: 40,
							max_current: 30,
							sensor_status: "OK",
							output: true,
							ontime: 345,
							laston: 0},
						"2": {
							lower_bound: 103,
							upper_bound: 203,
							temperature: 50,
							max_current: 30,
							sensor_status: "OK",
							output: false,
							ontime: 0,
							laston: 456},
					}
				} };

			process_json_message(data);
		}

		function process_state(state)
		{
			document.getElementById("State").innerHTML = "State: " + state;
			if((state === "Initial") || (state == "Idle"))
			{
				document.getElementById("TextInput").value = "";
			}
		}

		function connected_event()
		{
			document.getElementById("Connecting").style.display = "none";
			document.getElementById("Connected").style.display = "block";
		}

		function disconnected_event()
		{
			document.getElementById("Connecting").style.display = "block";
			document.getElementById("Connected").style.display = "none";
		}

		function isValid(obj)
		{
			for(var key in obj)
			{
				if(obj.hasOwnProperty(key))
					return true;
			}
			return false;
		}

		function send_channel_update(event)
		{
			console.log("send_channel_update: ", event);
			send_command("set_limit", [
				event.target.parentNode.parentNode.parentNode.id,
				event.target.id,
				event.target.value]);
		}

		function send_command(cmd, value)
		{
			const data = {};
			data[cmd] = value;

			const str = JSON.stringify(data);
			console.log("send: ", str);

			if(connection && connection.readyState == connection.OPEN)
			{
				connection.send(str);
			}
		}


	</script>
</head>

<body onload='open_web_socket()'>
	<div id='Connecting' style='display:none'>
		<div id='ConnectingContent'>
			<h3>Connecting</h3>
			<div class='Spinner'> <p style='display:none'> dummy </p> </div>
		</div>
	</div>
	<div id='Connected'>
		<div id='Main'>
			<div id='HeadLine'>
				<h3 >ThermoControl</h3>
			</div>
			<div id='content'>
				<div class='mode_div'> Manuell <label class="switch">
					  <input
					  	type="checkbox"
					  	id="ModeSwitch"
					  	onClick="switch_mode(event.target.checked)" />
					  <span class="slider round"></span>
					</label> Automatic </div>
				<div id="channels">Connecting ...</div>
			</div>
		</div>
	</div>
</body>
</html>)XML";
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
				[&context, this](WebsocketsClient& /*c*/, WebsocketsEvent type, WSInterfaceString)
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
				[&context, this](WebsocketsClient& /*client*/, WebsocketsMessage const& msg)
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
