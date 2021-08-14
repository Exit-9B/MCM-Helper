#include "Json/IHandler.h"
#include "Json/IHandler.inl"

IHandler::IHandler(ReaderHandler* master) : _master{ master }
{
}

bool IHandler::Null()
{
	return ReportError(ErrorType::UnexpectedNull);
}

bool IHandler::Bool(bool b)
{
	return ReportError(ErrorType::UnexpectedValueType, "boolean"sv, b);
}

bool IHandler::Int(int i)
{
	return ReportError(ErrorType::UnexpectedValueType, "integer"sv, i);
}

bool IHandler::Uint(unsigned i)
{
	return ReportError(ErrorType::UnexpectedValueType, "unsigned integer"sv, i);
}

bool IHandler::Int64(std::int64_t i)
{
	return ReportError(ErrorType::UnexpectedValueType, "64-bit integer"sv, i);
}

bool IHandler::Uint64(std::uint64_t i)
{
	return ReportError(ErrorType::UnexpectedValueType, "64-bit unsigned integer"sv, i);
}

bool IHandler::Double(double d)
{
	return ReportError(ErrorType::UnexpectedValueType, "double"sv, d);
}

bool IHandler::RawNumber(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return ReportError(ErrorType::UnexpectedValueType, "number"sv, str);
}

bool IHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return ReportError(ErrorType::UnexpectedValueType, "string"sv, str);
}

bool IHandler::StartObject()
{
	return ReportError(ErrorType::UnexpectedStartObject);
}

bool IHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return ReportError(ErrorType::UnexpectedKey, str);
}

bool IHandler::EndObject(SizeType memberCount)
{
	return ReportError(ErrorType::UnexpectedEndObject, memberCount);
}

bool IHandler::StartArray()
{
	return ReportError(ErrorType::UnexpectedStartArray);
}

bool IHandler::EndArray(SizeType elementCount)
{
	return ReportError(ErrorType::UnexpectedEndArray, elementCount);
}
