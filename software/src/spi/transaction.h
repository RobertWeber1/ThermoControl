#pragma once
#include <utility>
#include <tuple>
#include <stdint.h>
#include <cstring>
#include <type_traits>


namespace spi
{

enum class FailReason
{
	Aborted,
	Timeout
};


template<class>
struct MakeFuncSignature;


template<class ... Args>
struct MakeFuncSignature<std::tuple<Args...>>
{
	using type = std::function<void(Args...)>;
};


template<class Tuple>
using func_signature = typename MakeFuncSignature<Tuple>::type;

template<class, class, class>
struct StarterFunc;


template<class Derived, class OutputTuple, class ... Args>
struct StarterFunc<Derived, OutputTuple, std::tuple<Args...>>
{
	bool start(
		func_signature<OutputTuple> on_success,
		std::function<void(FailReason)> on_fail,
		Args const& ... args)
	{
		if(derived().index_ != derived().step_count())
		{
			return false;
		}

		std::memset(derived().template as<uint8_t>(), 0, derived().size());

		*derived().template as<std::tuple<Args...>>() = std::make_tuple(args...);

		derived().index_ = 0;
		derived().on_success_ = std::move(on_success);
		derived().on_fail_ = std::move(on_fail);
		return true;
	}


	Derived& derived()
	{
		return static_cast<Derived&>(*this);
	}
};


template<class SizeType, class InputTuple, class OutputTuple>
struct Transaction
: StarterFunc<Transaction<SizeType, InputTuple, OutputTuple>, OutputTuple, InputTuple>
{
	friend class StarterFunc<Transaction<SizeType, InputTuple, OutputTuple>, OutputTuple, InputTuple>;
	using size_type = SizeType;
	using input_func_t = func_signature<InputTuple>;
	using output_func_t = func_signature<OutputTuple>;

	static constexpr size_t step_count()
	{
		auto const io_size = std::max(sizeof(InputTuple), sizeof(OutputTuple));

		return
			(io_size / sizeof(SizeType) +
				((io_size % sizeof(SizeType) == 0) ? 0 : 1));
	}

	static constexpr size_t size()
	{
		return sizeof(SizeType) * step_count();
	}


	bool is_busy() const
	{
		return index_ != step_count();
	}

	size_t required_steps() const
	{
		return step_count() - index_;
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
			call(on_success_, *as<OutputTuple>());
		}
	}

	size_type get_spi_data() const
	{
		if(index_ == step_count())
		{
			return 0;
		}

		return as<SizeType>()[index_];
	}

	template<typename Function, typename Tuple, size_t ... I>
	auto call(Function && f, Tuple t, std::index_sequence<I ...>)
	{
	     return f(std::get<I>(t) ...);
	}

	template<typename Function, typename Tuple>
	auto call(Function && f, Tuple t)
	{
	    static constexpr auto size = std::tuple_size<Tuple>::value;
	    return call(std::forward<Function>(f), t, std::make_index_sequence<size>{});
	}
private:
	using storage_t =
		typename std::aligned_storage<
			size(),
			std::max({
				alignof(InputTuple),
				alignof(OutputTuple),
				alignof(SizeType)})>::type;

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

	output_func_t on_success_;
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

} //namespace spi
