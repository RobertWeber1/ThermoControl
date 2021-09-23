#include <system.h>
#include <SPI.h>
#include <SD.h>
#include <chrono>
//#include <EEPROM.h>

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
  
};

using Channel_t = thermo::Channel<Interface>;
using System_t = System<Interface, Interface, StorageInterface>;
using Sensor_t = thermo::Sensor;
using Output_t = thermo::Output<Interface>;
System_t * sys;

using io_t = thermo::SpiIO<Interface>;
io_t * test_io;

void setup() {
	Serial.begin(115200);
	Serial.println("\nStart");
	SPI.begin();
	SPI.setClockDivider(SPI_CLOCK_DIV16);

  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  if (!SD.begin(15)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset or reopen this Serial Monitor after fixing your issue!");
    //while (true);
  }
  
	sys = new System_t(
		std::vector<Channel_t>{
		Channel_t{
			thermo::LowerTemp(190.0f),
			thermo::UpperTemp(200.0f),
			Sensor_t(),
			Output_t(thermo::Pin(0), thermo::MaxCurrent(30.0f))},
		Channel_t{
			thermo::LowerTemp(190.0f),
			thermo::UpperTemp(200.0f),
			Sensor_t(),
			Output_t(thermo::Pin(1), thermo::MaxCurrent(30.0f))},
		Channel_t{
			thermo::LowerTemp(190.0f),
			thermo::UpperTemp(200.0f),
			Sensor_t(),
			Output_t(thermo::Pin(2), thermo::MaxCurrent(30.0f))},
		/*Channel_t{
			thermo::LowerTemp(190.0f),
			thermo::UpperTemp(200.0f),
			Sensor_t(),
			Output_t(thermo::Pin(3), thermo::MaxCurrent(30.0f))}*/},
		thermo::MaxCurrent(30.0f));
}

void loop()
{
	sys->process();
  digitalWrite(0, HIGH);
}
