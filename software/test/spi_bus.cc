#include <catch2/catch.hpp>
#include <utility>
#include <tuple>
#include <stdint.h>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <type_traits>

namespace spi
{

enum class FailReason
{
	Aborted,
	Timeout
};


template<class SizeType, class InputTuple, class OutputTuple>
struct Transaction
{
	using size_type = SizeType;

	static constexpr size_t size()
	{
		auto const io_size = std::max(sizeof(InputTuple), sizeof(OutputTuple));

		return
			sizeof(SizeType) *
			(io_size / sizeof(SizeType) +
				((io_size % sizeof(SizeType) == 0) ? 0 : 1));
	}

	static constexpr size_t step_count()
	{
		auto const io_size = std::max(sizeof(InputTuple), sizeof(OutputTuple));

		return
			(io_size / sizeof(SizeType) +
				((io_size % sizeof(SizeType) == 0) ? 0 : 1));
	}

	bool is_busy() const
	{
		// std::cout << "index: " << index_ << ", step_count: " << step_count() << std::endl;
		return index_ != step_count();
	}

	bool start(
		InputTuple const& input,
		std::function<void(OutputTuple const&)> on_success,
		std::function<void(FailReason)> on_fail)
	{
		if(index_ != step_count())
		{
			return false;
		}

		std::memset(as<uint8_t>(), 0, size());

		*as<InputTuple>() = input;

		// std::cout << "as<InputTuple>() ptr = " << (void*)as<InputTuple>() << std::endl;
		// std::cout << "start: InputTuple<0> = " << std::hex <<(uint64_t)std::get<0>(*as<InputTuple>()) << std::dec << std::endl;

		index_ = 0;
		on_success_ = std::move(on_success);
		on_fail_ = std::move(on_fail);
		return true;
	}

	void abort()
	{
		index_ = step_count();
		on_fail_(FailReason::Aborted);
	}

	void process_spi_data(size_type src)
	{
		if(index_ == step_count())
		{
			return;
		}

		as<SizeType>()[index_] = src;
		index_++;

		if(index_ == step_count())
		{
			on_success_(*as<OutputTuple>());
		}
	}

	size_type get_spi_data() const
	{
		if(index_ == step_count())
		{
			return 0;
		}
		// std::cout << "as<InputTuple>() ptr = " << (void*)as<SizeType>() << std::endl;

		// std::cout << "transaction get_spi_data("<< index_ <<"): " << std::hex << as<SizeType>()[index_] << std::endl;
		return as<SizeType>()[index_];
	}

private:
	using storage_t =
		typename std::aligned_storage<
			size(),
			std::max(
				alignof(InputTuple),
				std::max(
					alignof(OutputTuple),
					alignof(SizeType)))>::type;

	template<class T>
	T* as()
	{
		return reinterpret_cast<T*>(&storage_);
	}

	template<class T>
	T const* as() const
	{
		return reinterpret_cast<T const*>(&storage_);
	}

	std::function<void(OutputTuple const&)> on_success_;
	std::function<void(FailReason)> on_fail_;
	storage_t storage_;
	size_t index_ = step_count();
};

template<class SizeType, class MasterData, class DeviceData>
struct BaseTransaction
{
	using size_type = SizeType;
	using master_data = MasterData;
	using device_data = DeviceData;
};

template<class Base>
struct MakeMasterTransaction
{
	using type = Transaction<
		typename Base::size_type,
		typename Base::master_data,
		typename Base::device_data>;
};

template<class Base>
using make_master_transaction_t = typename MakeMasterTransaction<Base>::type;


template<class Base>
struct MakeDeviceTransaction
{
	using type = Transaction<
		typename Base::size_type,
		typename Base::device_data,
		typename Base::master_data>;
};

template<class Base>
using make_device_transaction_t = typename MakeDeviceTransaction<Base>::type;


//----------------------------------------------------------------------------//


struct Device
{

uint16_t hot_end_temp;
uint16_t internal_temp;
enum State : uint8_t
{
	Ok,
	OpenConnection,
	ShortToGND,
	ShortToVCC
}state;
bool output_state;

};


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

	template<class T>
	T get_tx_buffer_as(size_t index)
	{
		T dst;
		std::memcpy(&dst, &tx_buffer_[index], sizeof(T));
		return dst;
	}

	template<class T>
	T get_rx_buffer_as(size_t index)
	{
		T dst;
		std::memcpy(&dst, &rx_buffer_[index], sizeof(T));
		return dst;
	}

	static constexpr size_t buffer_size()
	{
		return (0 + ... + sizeof(typename Units::size_type));
	}

	static constexpr size_t tx_index_of(size_t i)
	{
		return calc_index_of_unit(i, std::make_index_sequence<sizeof ...(Units)>{});
	}

	static constexpr size_t rx_index_of(size_t i)
	{
		return calc_rx_index_of_unit(i, std::make_index_sequence<sizeof ...(Units)>{});
	}

	bool is_busy() const
	{
		return is_busy(std::make_index_sequence<sizeof ...(Units)>{});
	}

private:
	template<class T>
	void copy_from(size_t index, T const& src)
	{
		std::memcpy(
			&tx_buffer_[index],
			&src,
			sizeof(T));
	}

	template<class T>
	T copy_to(size_t index)
	{
		T dst;
		std::memcpy(
			&dst,
			&rx_buffer_[index],
			sizeof(T));
		return dst;
	}

	template<size_t ... Is>
	void fill_output_buffer(std::index_sequence<Is...>)
	{
		(copy_from(tx_index_of(Is), std::get<Is>(units_).get_spi_data()),...);
	}

	template<size_t ... Is>
	void consume_input_buffer(std::index_sequence<Is...>)
	{
		(std::get<Is>(units_).process_spi_data(
			copy_to<typename Units::size_type>(rx_index_of(Is))), ...);
	}

	template<size_t ... Is>
	static constexpr size_t calc_index_of_unit(size_t i, std::index_sequence<Is...>)
	{
		return ((width_in_byte<Units>()*(Is<i?1:0)) + ... );
	}

	template<size_t ... Is>
	static constexpr size_t calc_rx_index_of_unit(size_t i, std::index_sequence<Is...>)
	{
		return buffer_size() - ((width_in_byte<Units>()*(Is<=i?1:0)) + ... );
	}

	template<class T>
	T & as_unit()
	{
		return static_cast<T&>(*this);
	}

	template<class T>
	static constexpr size_t width_in_byte()
	{
		return sizeof(typename T::size_type);
	}

	template<size_t ... Is>
	bool is_busy(std::index_sequence<Is...>) const
	{
		return (std::get<Is>(units_).is_busy() or ...);
	}

private:
	uint8_t rx_buffer_[buffer_size()];
	uint8_t tx_buffer_[buffer_size()];
	Units_t units_;
};

template<class ... Args>
Bus<Args...> make_bus(Args& ... args)
{
	return Bus<Args...>(args...);
}

} //namespace spi

//------------------------------------------------------------------------------

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

	// puts("start in");
	REQUIRE(in.start(
		std::make_tuple(true, int(0x11223344), 456.78f),
		[](std::tuple<uint8_t> const& data)
		{
			// puts("in success");
			REQUIRE(std::get<0>(data) == 0xff);
		},
		[](spi::FailReason)
		{}));

	// puts("start out");
	REQUIRE(out.start(
		std::make_tuple(uint8_t(0xff)),
		[](std::tuple<bool, int, float> const& data)
		{
			// puts("out success");
			REQUIRE(std::get<0>(data) == true);
			REQUIRE(std::get<1>(data) == 0x11223344);
			REQUIRE(std::get<2>(data) == 456.78f);
		},
		[](spi::FailReason)
		{}));

	for(size_t i = 0; i<in.step_count(); ++i)
	{
		// printf("step: %d\n", i);
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

	// spi::Transaction<
	// 	uint16_t,
	// 	std::tuple<bool, int, float>,
	// 	std::tuple<uint8_t>> t1;

	// spi::Transaction<
	// 	uint16_t,
	// 	std::tuple<uint8_t>,
	// 	std::tuple<bool>> t2;

	auto bus = spi::make_bus(m1, m2);
	auto dev1 = spi::make_bus(d1);
	auto dev2 = spi::make_bus(d2);

	SECTION("not busy before start of tranaction")
	{
		REQUIRE_FALSE(bus.is_busy());
	}

	bool t1_succeeded = false;
	bool t1_failed = false;
	REQUIRE(m1.start(
		std::make_tuple(true, 0x11223344, 456.78f),
		[&t1_succeeded](std::tuple<uint8_t> const&)
		{
			t1_succeeded = true;
		},
		[&t1_failed](spi::FailReason)
		{
			t1_failed = true;
		}));

	SECTION("busy after first start of tranaction")
	{
		REQUIRE(bus.is_busy());
	}

	bool t2_succeeded = false;
	bool t2_failed = false;
	REQUIRE(m2.start(
		std::make_tuple(uint8_t(5)),
		[&t2_succeeded](std::tuple<bool> const&)
		{
			t2_succeeded = true;
		},
		[&t2_failed](spi::FailReason)
		{
			t2_failed = true;
		}));

	SECTION("still busy")
	{
		REQUIRE(bus.is_busy());
	}

	SECTION("get accumulated buffer size")
	{
		REQUIRE(bus.buffer_size() == 4);
	}


}
