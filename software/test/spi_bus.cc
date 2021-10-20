#include <catch2/catch.hpp>
#include <utility>
#include <tuple>
#include <stdint.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <type_traits>
#include <tuple>
#include <iostream>
#include <array>
#include <utility>
#include <spi/transaction.h>
#include <spi/bus.h>


struct TestUnit1
{
	using size_type = uint16_t;

	void process_spi_data(size_type src)
	{
		data = src;
	}

	size_type const& get_spi_data() const
	{
		return data;
	}

	size_type data = 0x0201;
};

template<class SizeType, SizeType Data>
struct TestUnit2
{
	using size_type = SizeType;

	void process_spi_data(size_type src)
	{
		data = src;
	}

	size_type const& get_spi_data() const
	{
		return data;
	}

	size_type data = Data;
};


TEST_CASE()
{
	TestUnit1 u1;
	TestUnit2<uint8_t, 0xaa> u2a;
	TestUnit2<uint16_t, 0xbbcc> u2b;
	TestUnit2<uint32_t, 0xddeeff11> u2c;

	auto bus = spi::make_bus(u1, u2a, u2b, u2c);

	SECTION("get accumulated buffer size")
	{
		REQUIRE(bus.buffer_size() == 9);
	}

	SECTION("check indices")
	{
		REQUIRE(bus.tx_index_of(0) == 0);
		REQUIRE(bus.tx_index_of(1) == 2);
		REQUIRE(bus.tx_index_of(2) == 3);
		REQUIRE(bus.tx_index_of(3) == 5);
	}

	SECTION("fill output buffer")
	{
		bus.fill_output_buffer();

		REQUIRE(bus.tx_buffer()[0] == 1);
		REQUIRE(bus.tx_buffer()[1] == 2);

		REQUIRE(bus.tx_buffer()[2] == 0xaa);

		REQUIRE(bus.tx_buffer()[3] == 0xcc);
		REQUIRE(bus.tx_buffer()[4] == 0xbb);

		REQUIRE(bus.tx_buffer()[5] == 0x11);
		REQUIRE(bus.tx_buffer()[6] == 0xff);
		REQUIRE(bus.tx_buffer()[7] == 0xee);
		REQUIRE(bus.tx_buffer()[8] == 0xdd);
	}

	SECTION("check rx indices")
	{
		REQUIRE(bus.rx_index_of(0) == 7);
		REQUIRE(bus.rx_index_of(1) == 6);
		REQUIRE(bus.rx_index_of(2) == 4);
		REQUIRE(bus.rx_index_of(4) == 0);
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

		REQUIRE(u1.data == 0x0908);
		REQUIRE(u2a.data == 7);
		REQUIRE(u2b.data == 0x0605);
		REQUIRE(u2c.data == 0x04030201);
	}
}

TEST_CASE("Transaction storage size")
{
	REQUIRE(sizeof(std::tuple<uint8_t, bool, float>) == 8);
	REQUIRE(sizeof(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t>) == 32);

	REQUIRE(
		spi::Transaction<
			uint8_t,
			std::tuple<uint8_t>,
			std::tuple<uint8_t>>::size() == 1);

	REQUIRE(
		spi::Transaction<
			uint16_t,
			std::tuple<uint8_t>,
			std::tuple<uint8_t>>::size() == 2);

	REQUIRE(
		spi::Transaction<
			uint8_t,
			std::tuple<uint8_t, uint8_t, uint8_t>,
			std::tuple<uint8_t>>::size() == 3);

	REQUIRE(
		spi::Transaction<
			uint16_t,
			std::tuple<uint8_t, uint8_t, uint8_t>,
			std::tuple<uint8_t>>::size() == 4);

	REQUIRE(
		spi::Transaction<
			uint16_t,
			std::tuple<uint8_t, uint8_t, uint8_t>,
			std::tuple<uint8_t>>::size() == 4);

	REQUIRE(
		spi::Transaction<
			uint64_t,
			std::tuple<uint8_t, uint8_t, uint8_t>,
			std::tuple<uint8_t>>::size() == 8);

	REQUIRE(
		spi::Transaction<
			uint8_t,
			std::tuple<uint8_t, uint8_t, float>,
			std::tuple<uint8_t>>::size() == 8);
}

TEST_CASE("get bytes from tranaction")
{
	spi::Transaction<
		uint16_t,
		std::tuple<bool, int, float>,
		std::tuple<uint8_t>> in;

	spi::Transaction<
		uint16_t,
		std::tuple<uint8_t>,
		std::tuple<bool, int, float>> out;

	REQUIRE_FALSE(in.is_busy());
	REQUIRE_FALSE(out.is_busy());

	REQUIRE(in.start(
		[](uint8_t a)
		{
			REQUIRE(a == 0xff);
		},
		[](spi::FailReason){},
		true, int(0x11223344), 456.78f));

	REQUIRE(out.start(
		[](bool a, int b, float c)
		{
			REQUIRE(a == true);
			REQUIRE(b == 0x11223344);
			REQUIRE(c == 456.78f);
		},
		[](spi::FailReason){},
		uint8_t(0xff)));


	for(size_t i = 0; i<in.step_count(); ++i)
	{
		REQUIRE(in.required_steps() == in.step_count() - i);

		auto const tmp = out.get_spi_data();
		out.process_spi_data(in.get_spi_data());
		in.process_spi_data(tmp);
	}
}

TEST_CASE("Use Transaction and Bus")
{
	using T1 = spi::BaseTransaction<
		uint16_t,
		std::tuple<bool, int, float>,
		std::tuple<uint8_t>>;

	using T2 = spi::BaseTransaction<
		uint16_t,
		std::tuple<uint8_t>,
		std::tuple<bool>>;

	spi::make_master_transaction_t<T1> m1;
	spi::make_master_transaction_t<T2> m2;

	spi::make_device_transaction_t<T1> d1;
	spi::make_device_transaction_t<T2> d2;

	auto bus = spi::make_bus(m1, m2);
	auto dev1 = spi::make_bus(d1);
	auto dev2 = spi::make_bus(d2);


	bool t1_succeeded = false;
	bool t1_failed = false;
	m1.start(
		[&t1_succeeded](uint8_t a)
		{
			t1_succeeded = true;
			REQUIRE(a == 32);
		},
		[&t1_failed](spi::FailReason)
		{
			t1_failed = true;
		},
		true, 0x11223344, 456.78f);

	bool t3_succeeded = false;
	bool t3_failed = false;
	d1.start(
		[&t3_succeeded](bool a, int b, float c)
		{
			t3_succeeded = true;
			REQUIRE(a == true);
			REQUIRE(b == 0x11223344);
			REQUIRE(c == 456.78f);
		},
		[&t3_failed](spi::FailReason)
		{
			t3_failed = true;
		},
		uint8_t(32));



	bool t2_succeeded = false;
	bool t2_failed = false;
	m2.start(
		[&t2_succeeded](bool a)
		{
			t2_succeeded = true;
			REQUIRE(a == false);
		},
		[&t2_failed](spi::FailReason)
		{
			t2_failed = true;
		},
		uint8_t(5));

	bool t4_succeeded = false;
	bool t4_failed = false;
	d2.start(
		[&t4_succeeded](uint8_t a)
		{
			t4_succeeded = true;
			REQUIRE(a == 5);
		},
		[&t4_failed](spi::FailReason)
		{
			t4_failed = true;
		},
		false);

	SECTION("successful transaction")
	{
		REQUIRE(bus.required_steps() == 6);
		REQUIRE(bus.buffer_size() == 4);


		auto const step_count = bus.required_steps();
		for(size_t i = 0; i<step_count; ++i)
		{
			bus.fill_output_buffer();
			dev1.fill_output_buffer();
			dev2.fill_output_buffer();

			std::memcpy(dev1.rx_buffer(), bus.tx_buffer(), bus.buffer_size()/2);
			std::memcpy(dev2.rx_buffer(), bus.tx_buffer()+bus.buffer_size()/2, bus.buffer_size()/2);

			std::memcpy(bus.rx_buffer(), dev2.tx_buffer(), dev2.buffer_size());
			std::memcpy(bus.rx_buffer()+dev2.buffer_size(), dev1.tx_buffer(), dev1.buffer_size());

			bus.consume_input_buffer();
			dev1.consume_input_buffer();
			dev2.consume_input_buffer();
		}

		REQUIRE(t1_succeeded);
		REQUIRE(t2_succeeded);
		REQUIRE(t3_succeeded);
		REQUIRE(t4_succeeded);
	}
}
