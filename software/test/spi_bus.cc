#include <catch2/catch.hpp>
#include <utility>
#include <tuple>
#include <stdint.h>


namespace spi
{

template<class ... Units>
struct Bus
{
	using Units_t = std::tuple<Units&...>;
	using Units_t_no_ref = std::tuple<Units...>;

	Bus(Units& ... units)
	: units_(units...)
	{}

	void fill_output_buffer()
	{
		fill_output_buffer(std::make_index_sequence<sizeof...(Units)>{});
	}

	void consume_input_buffer()
	{
		consume_input_buffer(std::make_index_sequence<sizeof...(Units)>{});
	}

	uint8_t * tx_buffer()
	{
		return tx_buffer_;
	}

	uint8_t * rx_buffer()
	{
		return rx_buffer_;
	}

	static constexpr size_t buffer_size()
	{
		return (0 + ... + Units::width_in_byte());
	}

	static constexpr size_t index_of(size_t i)
	{
		return calc_index_of_unit(i, std::make_index_sequence<sizeof ...(Units)>{});
	}

	static constexpr size_t rx_index_of(size_t i)
	{
		return calc_rx_index_of_unit(i, std::make_index_sequence<sizeof ...(Units)>{});
	}
private:
	template<size_t ... Is>
	void fill_output_buffer(std::index_sequence<Is...>)
	{
		size_t index = 0;
		(std::get<Is>(units_).fill_output_buffer(&tx_buffer_[index_of(index++)]),...);
	}

	template<size_t ... Is>
	void consume_input_buffer(std::index_sequence<Is...>)
	{
		size_t index = 0;
		(std::get<Is>(units_).consume_input_buffer(&rx_buffer_[rx_index_of(index++)]),...);
	}

	template<size_t ... Is>
	static constexpr size_t calc_index_of_unit(size_t i, std::index_sequence<Is...>)
	{
		return ((std::tuple_element_t<Is, Units_t_no_ref>::width_in_byte()*(Is<i?1:0)) + ... );
	}

	template<size_t ... Is>
	static constexpr size_t calc_rx_index_of_unit(size_t i, std::index_sequence<Is...>)
	{
		return buffer_size() - ((std::tuple_element_t<Is, Units_t_no_ref>::width_in_byte()*(Is<=i?1:0)) + ... );
	}

	template<class T>
	T & as_unit()
	{
		return static_cast<T&>(*this);
	}

private:
	uint8_t rx_buffer_[buffer_size()];
	uint8_t tx_buffer_[buffer_size()];
	Units_t units_;
};

} //namespace spi


struct TestUnit1
{
	static constexpr size_t width_in_byte()
	{
		return 2;
	}

	void consume_input_buffer(uint8_t const d[2])
	{
		data[0] = d[0];
		data[1] = d[1];
	}

	void fill_output_buffer(uint8_t * data)
	{
		data[0] = 1;
		data[1] = 2;
	}

	uint8_t data[2];
};


struct TestUnit2
{
	static constexpr size_t width_in_byte()
	{
		return 1;
	}

	void consume_input_buffer(uint8_t const d[1])
	{
		data = d[0];
	}

	void fill_output_buffer(uint8_t * data)
	{
		data[0] = 0xaa;
	}

	uint8_t data;
};

struct TestUnit3
{
	static constexpr size_t width_in_byte()
	{
		return 5;
	}

	void consume_input_buffer(uint8_t const d[5])
	{
		data[0] = d[0];
		data[1] = d[1];
		data[2] = d[2];
		data[3] = d[3];
		data[4] = d[4];
	}

	void fill_output_buffer(uint8_t * data)
	{
		data[0] = 5;
		data[1] = 6;
		data[2] = 7;
		data[3] = 8;
		data[4] = 9;
	}

	uint8_t data[5];
};

TEST_CASE()
{
	TestUnit1 u1;
	TestUnit2 u2a;
	TestUnit2 u2b;
	TestUnit3 u3;

	using Bus_t = spi::Bus<TestUnit1, TestUnit2, TestUnit2, TestUnit3>;
	Bus_t bus(u1, u2a, u2b, u3);

	SECTION("get accumulated buffer size")
	{
		REQUIRE(bus.buffer_size() == 9);
	}

	SECTION("check indices")
	{
		REQUIRE(Bus_t::index_of(0) == 0);
		REQUIRE(Bus_t::index_of(1) == 2);
		REQUIRE(Bus_t::index_of(2) == 3);
		REQUIRE(Bus_t::index_of(3) == 4);
	}

	SECTION("fill output buffer")
	{
		bus.fill_output_buffer();

		REQUIRE(bus.tx_buffer()[0] == 1);
		REQUIRE(bus.tx_buffer()[1] == 2);

		REQUIRE(bus.tx_buffer()[2] == 0xaa);

		REQUIRE(bus.tx_buffer()[3] == 0xaa);

		REQUIRE(bus.tx_buffer()[4] == 5);
		REQUIRE(bus.tx_buffer()[5] == 6);
		REQUIRE(bus.tx_buffer()[6] == 7);
		REQUIRE(bus.tx_buffer()[7] == 8);
		REQUIRE(bus.tx_buffer()[8] == 9);
	}

	SECTION("check rx indices")
	{
		REQUIRE(Bus_t::rx_index_of(0) == 7);
		REQUIRE(Bus_t::rx_index_of(1) == 6);
		REQUIRE(Bus_t::rx_index_of(2) == 5);
		REQUIRE(Bus_t::rx_index_of(4) == 0);
	}

	SECTION("consume input buffer")
	{
		bus.rx_buffer()[0] = 1;
		bus.rx_buffer()[1] = 2;
		bus.rx_buffer()[2] = 3;
		bus.rx_buffer()[3] = 4;
		bus.rx_buffer()[4] = 5;
		bus.rx_buffer()[5] = 6;
		bus.rx_buffer()[6] = 7;
		bus.rx_buffer()[7] = 8;
		bus.rx_buffer()[8] = 9;

		bus.consume_input_buffer();

		REQUIRE(u1.data[0] == 8);
		REQUIRE(u1.data[1] == 9);
		REQUIRE(u2a.data == 7);
		REQUIRE(u2b.data == 6);
		REQUIRE(u3.data[0] == 1);
		REQUIRE(u3.data[1] == 2);
		REQUIRE(u3.data[2] == 3);
		REQUIRE(u3.data[3] == 4);
		REQUIRE(u3.data[4] == 5);
	}
}
