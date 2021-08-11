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

	connection =
		new WebSocket('ws://192.168.4.1:8080/', ['Display']);
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
						console.log("send ---Heatbeat---");
						connection.send('---Heatbeat---');
					}, 1000);

			connected_event();
		};
	connection.onmessage =
		function (msg)
		{
			if(msg.data === '---Heatbeat---')
			{
				missed_heartbeats = 0;
				console.log("received heartbeat");
				connection.send('---Heatbeat---');
			}
			else
			{
				process_message(msg.data);
			}
		};
	connection.onerror =
		function ()
		{
			console.log('WebSocket closed, try reconnect in 500ms...');
			reconnect_timer = setTimeout(open_web_socket, 50000);
			clearInterval(heartbeat_interval);
		};
	connection.onclose =
		function ()
		{
			console.log('connecton closed by Server?');
			connection.onerror();
		};
}


function enable(id)
{
	document.getElementById(id).disabled = false;
	document.getElementById(id).style.display = 'block';
}

function disable(id)
{
	document.getElementById(id).style.display = 'none';
}

function process_message(raw_data)
{
	console.log("received message: ", raw_data);
	var data = JSON.parse(raw_data);

	if(data.automaitc_mode)
	{
		disable('AutoModeButton');
		enable('ManuModeButton');
	}
	else
	{
		enable('AutoModeButton');
		disable('ManuModeButton');
	}

	var channels = document.getElementById('channels');
	channels.innerHTML = "";

	for(const key in data.channels)
	{
		const ch_data = data.channels[key];
		console.log(key, ch_data);

		const channel = document.createElement('div');
		channel.className = "channel";
		channel.id = key;
		if(data.automaitc_mode == false)
		{
			var button = document.createElement('button');
			if(ch_data.output)
			{
				button.innerHTML = "OFF";
				button.addEventListener(
					"click",
					function()
					{
						console.log("turnOFF");
						send_command("switch", [key, "OFF"]);
					});
			}
			else
			{
				button.innerHTML = "ON";
				button.addEventListener(
					"click",
					function()
					{
						console.log("turnON");
						send_command("switch", [key, "ON"]);
					});
			}
			channel.appendChild(button);
		}
		else
		{
			const upper_p = document.createElement('p');
			const upper_input = document.createElement('input');
			upper_input.id = "UPPER";
			upper_input.type = "number";
			upper_input.onchange = send_channel_update;
			upper_input.value = ch_data.upper_bound;
			upper_p.innerHTML = "Obergrenze: ";
			upper_p.append(upper_input);

			const lower_p = document.createElement('p');
			const lower_input = document.createElement('input');
			lower_input.id = "LOWER";
			lower_input.type = "number";
			lower_input.onchange = send_channel_update;
			lower_input.value = ch_data.lower_bound;
			lower_p.innerHTML = "Untergrenze: ";
			lower_p.append(lower_input);

			channel.append(upper_p);
			channel.append(lower_p);
		}

		const temp = document.createElement('p');
		temp.append("Temperatur: ");
		temp.append(ch_data.temperature);
		temp.append(" °C");
		channel.appendChild(temp);

		const state = document.createElement('p');
		state.append("Sensorstaus: ");
		state.append(ch_data.sensor_status);
		channel.appendChild(state);

		const amps = document.createElement('p');
		amps.append("Max. Strom: ");
		amps.append(ch_data.max_current);
		amps.append(" A");
		channel.appendChild(amps);

		const output = document.createElement('p');
		if(ch_data.output)
		{
			output.append("Ausgang: AN");
		}
		else
		{
			output.append("Ausgang: AUS");
		}

		channel.appendChild(output);

		channels.appendChild(channel);
	}
}

function process_state(state)
{
	document.getElementById('State').innerHTML = 'State: ' + state;
	if((state === "Initial") || (state == "Idle"))
	{
		document.getElementById('TextInput').value = "";
	}
}

function connected_event()
{
	document.getElementById('Connecting').style.display = 'none';
	document.getElementById('Connected').style.display = 'block';
}

function disconnected_event()
{
	document.getElementById('Connecting').style.display = 'block';
	document.getElementById('Connected').style.display = 'none';
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
	send_command("set_limit", [
		event.target.parentNode.parentNode.id,
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
