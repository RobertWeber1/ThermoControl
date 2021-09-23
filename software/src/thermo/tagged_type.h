#pragma once

namespace thermo
{

template<class Tag, class Type>
struct Tagged
{
	using Self_t = Tagged<Tag, Type>;

	Tagged()
	{}

	explicit Tagged(Type val)
	: value_(val)
	{}

	operator Type() const
	{
		return value_;
	}

	Type get() const
	{
		return value_;
	}

	bool operator<(Self_t const& other)
	{
		return value_ < other.value_;
	}

private:
	Type value_;
};


#define MakeTagged(Name, Type) \
struct Name : Tagged<Name, Type> { using Tagged<Name, Type>::Tagged; }


} //namespace thermo
