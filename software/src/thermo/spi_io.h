#pragma once
#include <thermo/types.h>


namespace thermo
{


template<class Interface>
// 	uint8_t SelPin,
// 	uint8_t InBits,
	// uint8_t OutBits>
struct SpiIO
{
	// static void make_output(Pin pin)
	// {
	// 	//pinMode(pin.get(), OUTPUT);
	// }

	// static bool is_low(Pin pin)
	// {
	// 	return false;//digitalRead(pin.get()) == LOW;
	// }

	// static bool is_high(Pin pin)
	// {
	// 	return false;//digitalRead(pin.get()) == HIGH;
	// }

	// static void pull_low(Pin pin)
	// {
	// 	// digitalWrite(pin.get(), LOW);
	// }

	// static void pull_high(Pin pin)
	// {
	// 	// digitalWrite(pin.get(), HIGH);
	// }

	// static uint16_t transfer(uint16_t data)
	// {
	// 	return 0;//SPI.transfer16(data);
	// }

	SpiIO(Pin pin)
	: select_pin(pin)
	{

		Interface::make_output(select_pin);
		Interface::pull_low(select_pin);
	}

	uint16_t set(uint16_t value)
	{
		uint16_t result = Interface::transfer(value);
		Interface::pull_high(select_pin);
		delay(1000);
		Interface::pull_low(select_pin);
	}


private:
	Pin select_pin;

	// struct PinData
	// {};

	// static std::map<Pin, PinData> pin_data;
};

// template<class Interface, uint8_t SelPin>
// std::map<Pin, SpiIO<Interface, SelPin>::PinData>
// 	SpiIO<Interface, SelPin>::pin_data;

} //namespace thermo
