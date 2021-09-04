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

function setup(count)
{
	console.log("setup ", count, " channels");

	for(var index = 0; index < count; index++)
	{
		const channel = document.createElement('div');
		channel.className = "channel";
		channel.id = index;

		var off_button = document.createElement('button');
		off_button.id = index + "OFF";
		off_button.innerHTML = "OFF";
		off_button.addEventListener(
			"click",
			function()
			{
				console.log("turnOFF");
				send_command("switch", [index, "OFF"]);
			});
		channel.appendChild(off_button);

		var on_button = document.createElement('button');
		on_button.id = index + "ON";
		on_button.innerHTML = "ON";
		on_button.addEventListener(
			"click",
			function()
			{
				console.log("turnOFF");
				send_command("switch", [index, "OFF"]);
			});
		channel.appendChild(on_button);

		const upper_p = document.createElement('p');
		const upper_input = document.createElement('input');
		upper_input.id = index + "UPPER";
		upper_input.type = "number";
		upper_input.onchange = send_channel_update;
		upper_input.value = 0;
		upper_p.innerHTML = "Obergrenze: ";
		upper_p.append(upper_input);

		const lower_p = document.createElement('p');
		const lower_input = document.createElement('input');
		lower_input.id = index + "LOWER";
		lower_input.type = "number";
		lower_input.onchange = send_channel_update;
		lower_input.value = 0;
		lower_p.innerHTML = "Untergrenze: ";
		lower_p.append(lower_input);

		channel.append(upper_p);
		channel.append(lower_p);

		const p_temp = document.createElement('p');
		p_temp.append("Temperatur Sensor: ");

		const temp = document.createElement('a');
		temp.id = index + "temp";
		temp.innerHTML = "-";
		p_temp.appendChild(temp);
		channel.appendChild(p_temp);


		const p_output = document.createElement('p');
		p_output.append("Ausgang: ");

		const output = document.createElement('a');
		output.id = index + "output";
		output.innerHTML = "-";
		p_output.appendChild(output);

		channel.appendChild(p_output);

		var channels = document.getElementById('channels');
		channels.appendChild(channel);
	}
}

function process_message(raw_data)
{
	var message = JSON.parse(raw_data);

	if(message.type == "setup")
	{
		console.log(message);
    	setup(message.data);
	}
	else if(message.type == "data")
	{
		const data = message.data;
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

		for(const key in data.channels)
		{
			const ch_data = data.channels[key];

			const on = document.getElementById(key + "OFF");
			const off = document.getElementById(key + "ON");

			const upper = document.getElementById(key + "UPPER");
			const lower = document.getElementById(key + "LOWER");

			const temp = document.getElementById(key + "temp");
			const output = document.getElementById(key + "output");

			if(data.automaitc_mode == false)
			{
				on.style = "display:block;";
				off.style = "display:block;";
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

			if(upper.value != ch_data.upper_bound)
			{
				upper.value = ch_data.upper_bound;
			}


			if(lower.value != ch_data.lower_bound)
			{
				lower.value = ch_data.lower_bound;
			}

			if(ch_data.sensor_status == "OK")
			{
				temp.innerHTML = ch_data.temperature + " °C";
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
