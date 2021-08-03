#pragma once

#include <rapidjson/reader.h>
#include "Json/ReaderHandler.h"

class IHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	virtual ~IHandler() = default;

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

protected:
	enum class ErrorType
	{
		UnexpectedNull,
		UnexpectedBool,
		UnexpectedInt,
		UnexpectedUint,
		UnexpectedInt64,
		UnexpectedUint64,
		UnexpectedDouble,
		UnexpectedNumber,
		UnexpectedString,
		UnexpectedStartObject,
		UnexpectedKey,
		UnexpectedEndObject,
		UnexpectedStartArray,
		UnexpectedEndArray,
		InvalidKey,
		InvalidValue,
		MissingRequiredField,
	};

	IHandler(ReaderHandler* master);

	template <typename... Args>
	inline static bool ReportError(ErrorType errType, Args&&... args);

	template <typename... Args>
	inline static bool ReportError(std::string_view message, Args&&... args);

	ReaderHandler* _master;
};

#include "Json/IHandler.inl"