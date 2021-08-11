
// using namespace std::chrono_literals;

// namespace web
// {

// constexpr char main_page[] =
// "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
// R"HTML_STRING(
// <!DOCTYPE HTML>
// <html lang="en">
// <head>
// 	<meta charset="utf-8">
// 	<meta name='viewport' content='width=device-width'>
// 	<title>ThermoControl</title>
// 	<style>
// 		body{
// 			margin: 0;
// 			background: #EFEFEF;
// 		}
// 		#Connecting {
// 			width: 100%;
// 			background-color: grey;
// 			text-align: center;
// 			color: white;
// 		}
// 		#ConnectingContent {
// 			display: flex;
// 			justify-content: center;
// 			align-items: center;
// 		}
// 		.Spinner {
// 			margin-left: 5px;
// 			border: 6px solid #f3f3f3; /* Light grey */
// 			border-top: 6px solid #3498db; /* Blue */
// 			border-radius: 50%;
// 			width: 15px;
// 			height: 15px;
// 			animation: spin 4s linear infinite;
// 		}
// 		#Connected{
// 			margin:0;
// 		}
// 		#Main{
// 			display: flex;
// 			justify-content: center;
// 			align-items: center;
// 			flex-direction: column;
// 		}
// 		#HeadLine{
// 		f	margin: 0;
// 			border: 0;
// 			width: 100%;
// 			background-color: #4AA87E;
// 			text-align: center;
// 			color: white;
// 		}
// 		@keyframes spin {
// 			0% { transform: rotate(0deg); }
// 			100% { transform: rotate(360deg); }
// 		}
// 	</style>
// 	<script>
// 		var connection = null;
// 		var reconnect_timer = null;

// 		function open_web_socket()
// 		{
// 			console.log("try connect");
// 			if(reconnect_timer != null)
// 			{
// 				clearTimeout(reconnect_timer);
// 				reconnect_timer = null;
// 			}

// 			connection =
// 				new WebSocket('ws://'+location.hostname+':8080/', ['Display']);
// 			var heartbeat_interval = null;

// 			var missed_heartbeats = 0;
// 			function check_heartbeat()
// 			{
// 				clearInterval(heartbeat_interval);
// 				missed_heartbeats = 0;
// 				heartbeat_interval =
// 					setInterval(function()
// 						{
// 							missed_heartbeats++;
// 							if (missed_heartbeats >= 1)
// 							{
// 								clearInterval(heartbeat_interval);
// 								heartbeat_interval = null;
// 								connection.close();
// 							}
// 						}, 10000);
// 			}
// 			connection.onopen =
// 				function ()
// 				{
// 					missed_heartbeats = 0;
// 					connection.send('my date is ' + new Date());
// 					check_heartbeat();
// 					connected_event();
// 				};
// 			connection.onmessage =
// 				function (msg)
// 				{
// 					if(msg.data === '---Heatbeat---')
// 					{
// 						missed_heartbeats = 0;
// 						console.log("received heartbeat");
// 						connection.send('---Heatbeat---');
// 					}
// 					else
// 					{
// 						process_message(msg.data);
// 					}
// 				};
// 			connection.onerror =
// 				function ()
// 				{
// 					console.log('WebSocket closed, try reconnect in 500ms...');
// 					reconnect_timer = setTimeout(open_web_socket, 50000);
// 					// disconnected_event();
// 				};
// 			connection.onclose =
// 				function ()
// 				{
// 					console.log('connecton closed by Server?');
// 					connection.onerror();
// 				};
// 		}

// 		open_web_socket();

// 		function process_message(raw_data)
// 		{
// 			var obj = JSON.parse(raw_data);
// 			console.log("received data message: ", obj);
// 		}

// 		function process_state(state)
// 		{
// 			document.getElementById('State').innerHTML = 'State: ' + state;
// 			if((state === "Initial") | (state == "Idle"))
// 			{
// 				document.getElementById('TextInput').value = "";
// 			}
// 		}

// 		function connected_event()
// 		{
// 			document.getElementById('Connecting').style.display = 'none';
// 			document.getElementById('Connected').style.display = 'block';
// 		}

// 		function disconnected_event()
// 		{
// 			document.getElementById('Connecting').style.display = 'block';
// 			document.getElementById('Connected').style.display = 'none';
// 		}

// 		function isValid(obj)
// 		{
// 			for(var key in obj)
// 			{
// 				if(obj.hasOwnProperty(key))
// 					return true;
// 			}
// 			return false;
// 		}

// 		function send_update(tag, value)
// 		{
// 			const data = {};
// 			data[tag]=value;
// 			console.log("send: ", data);
// 			if(connection.readyState == connection.OPEN)
// 			{
// 				connection.send(data);
// 			}
// 		}

// 		function update_channel(data)
// 		{
// 			const channel = document.createElement('div');
// 			channel.id = data.channel;
// 			channel.className = "channel";

// 			const upper_p = document.createElement('p');
// 			const upper_input = document.createElement('input');
// 			upper_input.id = "upper_limit";
// 			upper_input.type = "number";
// 			upper_input.onchange = send_channel_update;
// 			upper_input.value = data.upper_limit;
// 			upper_p.innerHTML = "Obergrenze: ";
// 			upper_p.append(upper_input);

// 			const temp = document.createElement('p');
// 			temp.append("Temperatur: ");
// 			temp.append(data.temperature);
// 			temp.append(" °C");
// 			const lower_p = document.createElement('p');
// 			const lower_input = document.createElement('input');
// 			lower_input.id = "lower_limit";
// 			lower_input.type = "number";
// 			lower_input.onchange = send_channel_update;
// 			lower_input.value = data.lower_limit;
// 			lower_p.innerHTML = "Untergrenze: "
// 			lower_p.append(lower_input);

// 			channel.append(upper_p);
// 			channel.append(temp);
// 			channel.append(lower_p);

// 			console.log("new_channel: ", channel);

// 			const element = document.getElementById(data.channel);
// 			if(element)
// 			{
// 				element.innerHTML = "";
// 				element.append(channel);
// 			}
// 			else
// 			{
// 				const content = document.getElementById("content");

// 				content.append(channel);
// 			}
// 		}

// 		function send_channel_update(event)
// 		{
// 			const data = {};
// 			data['channel'] = event.target.parentNode.parentNode.id;
// 			//TODO convert value into number
// 			data[event.target.id] = event.target.value;

// 			console.log("send: ", data);

// 			if(connection.readyState == connection.OPEN)
// 			{
// 				connection.send(data);
// 			}
// 		}
// 		//----------------------------------------------------------------------
// 	</script>
// </head>
// <body>
// 	<div id='Connecting' style='display:none'>
// 		<div id='ConnectingContent'>
// 			<h3>Connecting</h3>
// 			<div class='Spinner'> </div>
// 		</div>
// 	</div>
// 	<div id='Connected' >
// 		<div id='Main'>
// 			<div id='HeadLine'>
// 				<h3 >ThermoControl</h3>
// 			</div>
// 			<div id='content'>
// 				<div class="channel" id="channel_1">
// 					<p>Obergrenze:
// 						<input
// 							type="number"
// 							id="upper_limit"
// 							onchange="send_channel_update(event)">
// 					</p>
// 					<p>Temperatur: <a id="hot_end_temp">--</a>°C</p>
// 					<p>Untergrenze:
// 						<input
// 							type="number"
// 							id="lower_limit"
// 							onchange="send_channel_update(event)">
// 					</p>
// 				</div>
// 			</div>
// 		</div>
// 	</div>
// 	<div class='footer' style="position:fixed; left:0; bottom:0; width:100%; background-color:grey; text-align:center;">
// 		<button onclick="update_channel({'channel': 'channel_1', 'upper_limit': 123, 'lower_limit': 123, 'temperature':43.5})">update Channel1</button>
// 		<button onclick="update_channel({'channel': 'channel_2', 'upper_limit': 1000, 'lower_limit': 1, 'temperature':123})">update Channel2</button>
// 	</div>
// </body>
// </html>
// )HTML_STRING";


// struct JsonStream
// {
// 	char const* begin;
// 	char const* end;

// 	JsonStream(websockets::WebsocketsMessage const& msg)
// 	: begin(msg.c_str())
// 	, end(msg.c_str()+msg.length())
// 	{}

// 	bool eof() const
// 	{
// 		return begin >= end;
// 	}

// 	size_t skip_seperator()
// 	{
// 		char const* start = begin;

// 		while(
// 			not eof() and
// 			(*begin == '{' or
// 			 *begin == '}' or
// 			 *begin == '[' or
// 			 *begin == ']' or
// 			 *begin == ' ' or
// 			 *begin == ':' or
// 			 *begin == ','))
// 		{
// 			++begin;
// 		}

// 		return begin - start;
// 	}

// 	size_t skip_number()
// 	{
// 		char const* start = begin;

// 		while(not eof() and *begin >= '0' and *begin <= '9')
// 		{
// 			++begin;
// 		}

// 		return begin - start;
// 	}

// 	size_t skip_text()
// 	{
// 		char const* start = begin;

// 		while(
// 			not eof() and
// 			((*begin >= 'a' and *begin <= 'z') or
// 			 (*begin >= 'A' and *begin <= 'Z')))
// 		{
// 			++begin;
// 		}

// 		return begin - start;
// 	}

// 	friend JsonStream& operator>>(JsonStream& stream, std::string & value)
// 	{
// 		stream.skip_separator();
// 		value = std::string(stream.begin, stream.skip_text());
// 		return stream;
// 	}

// 	friend JsonStream& operator>>(JsonStream& stream, int & value)
// 	{
// 		stream.skip_separator();
// 		value = std::atoi(stream.begin)
// 		stream.skip_text();
// 		return stream;
// 	}
// };


// template<
// 	class Derived,
// 	uint8_t WSClientCount = 2,
// 	uint16_t HttpPort = 80,
// 	uint16_t WsPort = 8080>
// struct Server
// {
// 	using milliseconds = std::chrono::milliseconds;
// 	using time_point = std::chrono::steady_clock::time_point;

// 	Server()
// 	{
// 		Serial.println(WiFi.softAP("AndroidAP1", "androidap") ? "Ready" : "Failed!");

// 		Serial.print("AP IP address: ");
// 		Serial.println(WiFi.softAPIP());
// 		Serial.println("WebServer Started");

// 		ws_server.listen(WsPort);
// 		web_server.begin();
// 	}

// 	void process()
// 	{
// 		if(sec <= std::chrono::steady_clock::now())
// 		{
// 			++count;
// 			sec = std::chrono::steady_clock::now() + milliseconds(1000);
// 		}
// 		process_web_server();
// 		process_ws_server();
// 		process_ws_clients();
// 	}

// 	void publish(String const& data)
// 	{
// 		for(auto & client : ws_clients)
// 		{
// 			client.send(data);
// 		}
// 	}

// private:
// 	void process_web_server()
// 	{
// 		auto client = web_server.available();
// 		if(client)
// 		{
// 			Serial.println("deliver web page");
// 			client.println(main_page);
// 		}
// 	}

// 	void process_ws_server()
// 	{
// 		if(not ws_server.poll())
// 		{
// 			return;
// 		}

// 		int slot = get_ws_slot();
// 		if(slot == -1)
// 		{
// 			return;
// 		}

// 		Serial.println("ws_server.acccept");
// 		ws_clients[slot].init(slot, derived(), ws_server.accept());
// 		Serial.print("[");
// 		Serial.print(slot);
// 		Serial.println("] WS-Client connected");
// 	}

// 	void process_ws_clients()
// 	{
// 		for(auto & client : ws_clients)
// 		{
// 			client.process();
// 		}
// 	}

// 	int get_ws_slot()
// 	{
// 		for(int i= 0; i<WSClientCount; ++i)
// 		{
// 			if(not ws_clients[i].available())
// 			{
// 				return i;
// 			}
// 		}

// 		return -1;
// 	}

// private:
// 	WiFiServer web_server = WiFiServer(HttpPort);
// 	websockets::WebsocketsServer ws_server;

// public:
// 	struct WSClient : websockets::WebsocketsClient
// 	{
// 		void print(const char* s)
// 		{
// 			Serial.print("[");
// 			Serial.print(slot_);
// 			Serial.print("] ");
// 			Serial.println(s);
// 		}
// 		template<class Context>
// 		void init(int slot, Context & context, const websockets::WebsocketsClient& other)
// 		{
// 			slot_ = slot;
// 			static_cast<websockets::WebsocketsClient&>(*this) = other;
// 			using namespace websockets;

// 			onEvent(
// 				[&, &context](WebsocketsClient& c, WebsocketsEvent type, WSInterfaceString)
// 				{
// 					switch(type)
// 					{
// 					case WebsocketsEvent::ConnectionOpened:

// 						print("connected");
// 						context.client_connected(*this);
// 						break;

// 					case WebsocketsEvent::ConnectionClosed:
// 						print("disconnected");
// 						heartbeat_deadline = time_point::max();
// 						heartbeat_response_deadline = time_point::max();
// 						break;

// 					case WebsocketsEvent::GotPing:
// 						print("ping");
// 						break;

// 					case WebsocketsEvent::GotPong:
// 						print("pong");
// 						break;
// 					}
// 				});

// 			onMessage(
// 				[&, &context](WebsocketsClient& client, WebsocketsMessage const& msg)
// 				{
// 					if(msg.data() == "---Heatbeat---")
// 					{
// 						print("received ---Heatbeat---");
// 						heartbeat_response_deadline = time_point::max();
// 						heartbeat_deadline =
// 							hb_request_interval +
// 							std::chrono::steady_clock::now();
// 					}
// 					else
// 					{
// 						context.received_message(msg);
// 					}
// 				});

// 			heartbeat_deadline = std::chrono::steady_clock::now() + 2s;
// 			heartbeat_response_deadline = time_point::max();
// 		}

// 		void process()
// 		{
// 			if(available())
// 			{
// 				poll();
// 				if(heartbeat_deadline <= std::chrono::steady_clock::now())
// 				{
// 					print("xxxxxxxxxx send heartbeat");
// 					if(send("---Heatbeat---"))
// 					{
// 						heartbeat_response_deadline =
// 							std::chrono::steady_clock::now() + hb_response_interval;
// 						heartbeat_deadline = time_point::max();
// 						print("\t\theartbeat_response_deadline resetted");
// 					}
// 					else
// 					{
// 						print("\t\tsend heartbeat failed");
// 						close();
// 						heartbeat_response_deadline = time_point::max();
// 						heartbeat_deadline = time_point::max();
// 					}
// 				}
// 				else if(heartbeat_response_deadline <= std::chrono::steady_clock::now())
// 				{
// 					print("heartbeat timed out!");
// 					close();
// 					heartbeat_deadline = time_point::max();
// 					time_point heartbeat_deadline = time_point::max();
// 				}
// 			}
// 		}

// 	private:
// 		time_point heartbeat_deadline = std::chrono::steady_clock::now();
// 		time_point heartbeat_response_deadline = time_point::max();

// 		int slot_ = 0;
// 		const milliseconds hb_request_interval = milliseconds(5000);
// 		const milliseconds hb_response_interval = milliseconds(1000);
// 	};

// private:
// 	Derived& derived()
// 	{
// 		return static_cast<Derived&>(*this);
// 	}

// private:
// 	std::array<WSClient, WSClientCount> ws_clients;

// 	const milliseconds animation_delay = milliseconds(200);
// 	time_point frame_deadline = std::chrono::steady_clock::now();
// 	time_point sec = std::chrono::steady_clock::now();
// 	int count = 0;
// };

// } //namespace web


// namespace thermo
// {

// using time_point = std::chrono::steady_clock::time_point;

// struct Filter
// {
// 	Filter(float f)
// 	: factor(f)
// 	{}

// 	float value(float next)
// 	{
// 		return previous = factor * previous + (1-factor) * next;
// 	}

// 	float value() const
// 	{
// 		return previous;
// 	}

// private:
// 	float previous = 0.0f;
// 	float factor;
// };

// struct Pin
// {
// 	explicit Pin(int val)
// 	: value_(val)
// 	{}

// 	operator int() const
// 	{
// 		return value_;
// 	}

// private:
// 	int value_;
// };


// struct Sensor
// {
// 	Sensor(Pin select_pin, float filter_factor = 0.96)
// 	: filter_hotend_(filter_factor)
// 	, filter_internal_(filter_factor)
// 	, select_pin_(select_pin)
// 	{
// 		pinMode(select_pin_, OUTPUT);
// 		digitalWrite(select_pin_, HIGH);
// 	}

// 	void process()
// 	{
// 		digitalWrite(select_pin_, LOW);
// 		const int16_t first_word = SPI.transfer16(uint16_t(0));
// 		const int16_t second_word = SPI.transfer16(uint16_t(0));
// 		digitalWrite(select_pin_, HIGH);

// 		bool const fault = first_word & 1;
// 		open_conn_ = second_word & 1;
// 		short_to_gnd_ = second_word & 2;
// 		short_to_vcc_ = second_word & 4;

// 		if(not fault)
// 		{
// 			filter_hotend_.value((first_word>>2)/4.0f);
// 			filter_internal_.value((second_word>>4)/16.0f);
// 		}
// 	}

// 	float hot_end_temperature() const
// 	{
// 		return filter_hotend_.value();
// 	}

// 	float internal_temperatur() const
// 	{
// 		return filter_internal_.value();
// 	}

// 	bool has_open_connection() const
// 	{
// 		return open_conn_;
// 	}

// 	bool has_short_to_gnd() const
// 	{
// 		return short_to_gnd_;
// 	}

// 	bool has_short_to_vcc() const
// 	{
// 		return short_to_vcc_;
// 	}

// private:
// 	Filter filter_hotend_;
// 	Filter filter_internal_;
// 	int select_pin_;
// 	bool open_conn_ = false;
// 	bool short_to_gnd_ = false;
// 	bool short_to_vcc_ = false;
// };

// struct Output
// {
// 	Output(int pin, float current = 30.0)
// 	: pin_(pin)
// 	, current_(current)
// 	{
// 		pinMode(pin_, OUTPUT);
// 		digitalWrite(pin_, HIGH);
// 	}

// 	time_point last_activation() const
// 	{
// 		return last_activation_;
// 	}

// 	time_point last_deactivation() const
// 	{
// 		return last_deactivation_;
// 	}

// 	float current() const
// 	{
// 		return
// 			(digitalRead(pin_) == LOW)
// 			? current_
// 			: 0;
// 	}

// 	float requirred_current() const
// 	{
// 		return current_;
// 	}

// 	bool is_deactivated() const
// 	{
// 		return digitalRead(pin_) == HIGH;
// 	}

// 	void activate()
// 	{
// 		last_activation_ = std::chrono::steady_clock::now();
// 		digitalWrite(pin_, LOW);
// 	}

// 	void deactivate()
// 	{
// 		last_deactivation_ = std::chrono::steady_clock::now();
// 		digitalWrite(pin_, HIGH);
// 	}

// private:
// 	int pin_;
// 	float current_;
// 	time_point last_activation_;
// 	time_point last_deactivation_;
// };

// struct Channel
// {
// 	float requirred_temperature;
// 	float hysteresis;
// 	Sensor sensor;
// 	Output output;
// };

// struct Control
// {
// 	Control(std::vector<Channel> const& channels, float max_current)
// 	: channels_(channels)
// 	{}

// 	void process()
// 	{
// 		float actual_current = 0.0f;

// 		for(auto & channel : channels_)
// 		{
// 			channel.sensor.process();
// 			if(channel.sensor.hot_end_temperature() > channel.requirred_temperature + channel.hysteresis)
// 			{
// 				channel.output.deactivate();
// 			}
// 			actual_current += channel.output.current();
// 		}

// 		std::vector<Channel*> candidates_for_activation;
// 		for(auto & channel : channels_)
// 		{
// 			if((channel.sensor.hot_end_temperature() < channel.requirred_temperature - channel.hysteresis) and
// 					channel.output.is_deactivated())
// 			{
// 				candidates_for_activation.push_back(&channel);
// 			}
// 		}

// 		std::sort(
// 			candidates_for_activation.begin(),
// 			candidates_for_activation.end(),
// 			[](auto & lhs, auto & rhs)
// 			{
// 				return lhs->output.last_deactivation() > rhs->output.last_deactivation();
// 			});

// 		for(auto & channel : candidates_for_activation)
// 		{
// 			if(actual_current + channel->output.requirred_current() < max_current_)
// 			{
// 				channel->output.activate();
// 				actual_current += channel->output.requirred_current();
// 			}
// 		}
// 	}

// private:
// 	float max_current_;
// 	std::vector<Channel> channels_;
// };



// } //namespace thermo

// struct System
// : web::Server<System>
// , thermo::Control
// {
// 	using Server_t = web::Server<System>;
// 	using Thermo_t = thermo::Control;

// 	using Thermo_t::Control;

// 	// System()
// 	// : Thermo_t({
// 	// 	thermo::Channel{30.0f, 10.0f, thermo::Sensor(thermo::Pin(16)), thermo::Output(LED_BUILTIN)},
// 	// 	thermo::Channel{30.0f, 10.0f, thermo::Sensor(thermo::Pin(15)), thermo::Output(22)},
// 	// 	thermo::Channel{30.0f, 10.0f, thermo::Sensor(thermo::Pin(5)), thermo::Output(23)}},
// 	// 	30.0f)
// 	// {}

// 	void process()
// 	{
// 		Server_t::process();
// 		Thermo_t::process();
// 		if(std::chrono::steady_clock::now() >= next_gui_update)
// 		{
// 			next_gui_update = std::chrono::steady_clock::now() + 1s;
// 			Server_t::publish("[\"Test\"]");
// 		}
// 	}

// 	void client_connected(Server_t::WSClient & client)
// 	{
// 		Serial.println("System new client connected");
// 		client.send("{\"Server\": 234}");
// 		Server_t::publish("[\"initial_message\"]");
// 	}

// 	void received_message(websockets::WebsocketsMessage const& msg)
// 	{
// 		Serial.println("System received message");
// 		Serial.println(msg.c_str());


// 	}

// private:
// 	time_point next_gui_update = std::chrono::steady_clock::now();
// };
// #include <SPI.h>

// struct PinInterface
// {
// 	static void make_output(Pin pin)
// 	{
// 		pinMode(pin, OUTPUT);
// 	}

// 	static bool is_low(Pin pin)
// 	{
// 		return digitalRead(pin) == LOW;
// 	}

// 	static bool is_high(Pin pin)
// 	{
// 		return digitalRead(pin) == HIGH;
// 	}

// 	static void pull_low(Pin pin)
// 	{
// 		digitalWrite(pin, LOW);
// 	}

// 	static void pull_high(Pin pin)
// 	{
// 		digitalWrite(pin, HIGH);
// 	}
// };

// struct SpiInterface
// {
// 	static uint16_t transfer(uint16_t data)
// 	{
// 		return SPI.transfer16(data)
// 	}
// };

// using Channel_t = thermo::Channel<PinInterfece, SpiInterface>;
// using System_t = System<PinInterfece, SpiInterface>;

// System_t * sys;

// void setup() {
// 	Serial.begin(115200);
// 	Serial.println("Start");
// 	SPI.begin();
// 	SPI.setClockDivider(SPI_CLOCK_DIV2);
// 	sys = new System_t(
// 		Channel_t{20.0f, 40.0f, thermo::Sensor(thermo::Pin(16)), thermo::Output(LED_BUILTIN)},
// 		Channel_t{20.0f, 40.0f, thermo::Sensor(thermo::Pin(15)), thermo::Output(22)},
// 		Channel_t{20.0f, 40.0f, thermo::Sensor(thermo::Pin(5)), thermo::Output(23)},
// 		30.0f);
// }


// void loop() {
// 	sys->process();
// }
