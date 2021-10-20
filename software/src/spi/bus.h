#pragma once
#include <utility>
#include <tuple>
#include <stdint.h>
#include <cstring>
#include <type_traits>


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

	size_t required_steps() const
	{
		return required_steps(std::make_index_sequence<sizeof ...(Units)>{});
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

	template<size_t ... Is>
	size_t required_steps(std::index_sequence<Is...>) const
	{
		return std::max({std::get<Is>(units_).required_steps()...});
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
