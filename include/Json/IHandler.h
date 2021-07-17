#pragma once

#include <rapidjson/reader.h>

class IHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	virtual bool Null();
	virtual bool Bool(bool b);
	virtual bool Int(int i);
	virtual bool Uint(unsigned i);
	virtual bool Int64(std::int64_t i);
	virtual bool Uint64(std::uint64_t i);
	virtual bool Double(double d);
	virtual bool RawNumber(const Ch* str, SizeType length, bool copy);
	virtual bool String(const Ch* str, SizeType length, bool copy);
	virtual bool StartObject();
	virtual bool Key(const Ch* str, SizeType length, bool copy);
	virtual bool EndObject(SizeType memberCount);
	virtual bool StartArray();
	virtual bool EndArray(SizeType elementCount);
};