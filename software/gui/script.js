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
	input.onClick = on_click;

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
	console.log("process_json_message: ", message);
	if(message.type == "setup")
	{
		console.log(message);
    	setup(message.data);
	}
	else if(message.type == "data")
	{
		const data = message.data;

		console.log(data);

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

			upper_val.innerHTML = ch_data.upper_bound + " °C";
			lower_val.innerHTML = ch_data.lower_bound + " °C";
			on_time_val.innerHTML = ch_data.on_time + " s";

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
