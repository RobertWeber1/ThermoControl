#include <system.h>
#include <SPI.h>
#include <SD.h>
#include <chrono>


struct Interface
{
	static void make_output(thermo::Pin pin)
	{
		Serial.print("make output: ");
		Serial.println(pin.get());
		pinMode(pin.get(), OUTPUT);
	}

	static bool is_low(thermo::Pin pin)
	{
		return digitalRead(pin.get()) == LOW;
	}

	static bool is_high(thermo::Pin pin)
	{
		return digitalRead(pin.get()) == HIGH;
	}

	static void pull_low(thermo::Pin pin)
	{
		digitalWrite(pin.get(), LOW);
	}

	static void pull_high(thermo::Pin pin)
	{
		digitalWrite(pin.get(), HIGH);
	}

	static uint16_t transfer(uint16_t data)
	{
		return SPI.transfer16(data);
	}

	static void toggle(thermo::Pin pin)
	{
		if(is_low(pin))
		{
			pull_high(pin);
		}
		else
		{
			pull_low(pin);
		}
	}
};

struct StorageInterface
{
	StorageInterface()
	{
		if (!SD.begin(15)) {
			Serial.println("initialization failed. Things to check:");
			Serial.println("1. is a card inserted?");
			Serial.println("2. is your wiring correct?");
			Serial.println("3. did you change the chipSelect pin to match your shield or module?");
			Serial.println("Note: press reset or reopen this Serial Monitor after fixing your issue!");
		}
	}

	template<class T>
	static T read(char const* filename, T default_value)
	{
		if(SD.exists(filename))
		{
			File config = SD.open(filename, FILE_READ);
			config.read(reinterpret_cast<uint8_t *>(&default_value), sizeof(T));
			config.close();
			Serial.println("read data from file");
		}
		else
		{
			Serial.print("read - failed to open: ");
			Serial.print(filename);
			Serial.print(", use default value.");
		}

		return default_value;
	}

	static bool file_exists(char const* filename)
	{
		return SD.exists(filename);
	}

	template<class T>
	static T read(char const* filename)
	{
		T data;
		if(SD.exists(filename))
		{
			File config = SD.open(filename, FILE_READ);
			config.read(reinterpret_cast<uint8_t *>(&data), sizeof(T));
			config.close();
			Serial.println("read data from file");
		}
		else
		{
			Serial.print("read - failed to open: ");
			Serial.println(filename);
		}

		return data;
	}

	template<class T>
	static void write(char const* filename, T const& value)
	{
		File config = SD.open(filename, FILE_WRITE);
		if(config)
		{
			Serial.println("write config to file");
			if(config.seek(0))
			{
				Serial.println("seeked to begin of file");
			}
			else
			{
				Serial.println("seeked to begin of file");
			}
			Serial.print(config.write(reinterpret_cast<char const*>(&value), sizeof(T)));
			Serial.println(" bytes written;");
			config.close();
		}
		else
		{
			Serial.print("write - failed to open: ");
			Serial.println(filename);
		}
	}
};

using Channel_t = thermo::Channel<Interface>;
using System_t = System<Interface, Interface, StorageInterface, 2>;
using Sensor_t = thermo::Sensor;
using Output_t = thermo::Output<Interface>;
System_t * sys;

void setup() {
	Serial.begin(115200);
	Serial.println("\nStart");
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV128);

	sys = new System_t();
}

void loop()
{
	sys->process();
}
