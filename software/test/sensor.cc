#include <catch2/catch.hpp>
#include <thermo/sensor.h>
#include <queue>


namespace
{

template<int Pin, bool Value>
struct PinInterface
{
	static bool is_low(thermo::Pin pin)
	{
		check(pin.get());
		return Value;
	}

	static bool is_high(thermo::Pin pin)
	{
		check(pin.get());
		return Value;
	}

	static void make_output(thermo::Pin pin)
	{
		check(pin.get());
		results.push(__FUNCTION__);
	}

	static void pull_low(thermo::Pin pin)
	{
		check(pin.get());
		results.push(__FUNCTION__);
	}

	static void pull_high(thermo::Pin pin)
	{
		check(pin.get());
		results.push(__FUNCTION__);
	}

	static void check(int pin)
	{
		if(pin != Pin)
		{
			throw std::logic_error(
				"Pin should be " + std::to_string(Pin) +
				" not " + std::to_string(pin) + "!");
		}
	}

/////////////
	static void clear_results()
	{
		results = std::queue<std::string>();
	}

	static std::string get_result()
	{
		if(results.empty())
		{
			return "No result!";
		}

		const std::string tmp = results.front();
		results.pop();
		return tmp;
	}

private:
	static std::queue<std::string> results;
};

template<int A, bool B>
std::queue<std::string> PinInterface<A,B>::results;


struct SpiInterface
{
	static uint16_t transfer(uint16_t data)
	{
		return 0;
		// return SPI.transfer16(data)
	}


};


} //namespace


TEST_CASE("sensor")
{
	using P2flase = PinInterface<2, false>;
	P2flase::clear_results();

	thermo::Sensor<P2flase, SpiInterface> sensor(thermo::Pin(2));

	SECTION("select pin setup")
	{
		REQUIRE(P2flase::get_result() == "make_output");
		REQUIRE(P2flase::get_result() == "pull_high");
	}

	SECTION("process")
	{
		P2flase::clear_results();
		sensor.process();
		REQUIRE(P2flase::get_result() == "pull_low");
		REQUIRE(P2flase::get_result() == "pull_high");
	}
}
