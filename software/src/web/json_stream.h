#pragma once
#include <string>

namespace web
{

struct JsonStream
{
	char const* begin;
	char const* end;

	JsonStream(char const* data, size_t size)
	: begin(data)
	, end(data + size)
	{}

	bool eof() const
	{
		return begin >= end;
	}

	size_t skip_seperator()
	{
		char const* start = begin;

		while(
			not eof() and
			(*begin == '{' or
			 *begin == '}' or
			 *begin == '[' or
			 *begin == ']' or
			 *begin == ' ' or
			 *begin == ':' or
			 *begin == '"' or
			 *begin == '\'' or
			 *begin == ','))
		{
			++begin;
		}

		return begin - start;
	}

	size_t skip_number()
	{
		char const* start = begin;

		while(not eof() and *begin >= '0' and *begin <= '9')
		{
			++begin;
		}

		return begin - start;
	}

	size_t skip_text()
	{
		char const* start = begin;

		while(
			not eof() and
			((*begin >= 'a' and *begin <= 'z') or
			 (*begin >= 'A' and *begin <= 'Z') or
			 (*begin >= '0' and *begin <= '9') or
			 *begin == '_'))
		{
			++begin;
		}

		return begin - start;
	}

	friend JsonStream& operator>>(JsonStream& stream, std::string & value)
	{
		stream.skip_seperator();
		value = std::string(stream.begin, stream.skip_text());
		return stream;
	}

	friend JsonStream& operator>>(JsonStream& stream, int & value)
	{
		stream.skip_seperator();
		value = std::atoi(stream.begin);
		stream.skip_text();
		return stream;
	}
};

} //namespace web
