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
	return ReportError(ErrorType::UnexpectedBool, b);
}

bool IHandler::Int(int i)
{
	return ReportError(ErrorType::UnexpectedInt, i);
}

bool IHandler::Uint(unsigned i)
{
	return ReportError(ErrorType::UnexpectedUint, i);
}

bool IHandler::Int64(std::int64_t i)
{
	return ReportError(ErrorType::UnexpectedInt64, i);
}

bool IHandler::Uint64(std::uint64_t i)
{
	return ReportError(ErrorType::UnexpectedUint64, i);
}

bool IHandler::Double(double d)
{
	return ReportError(ErrorType::UnexpectedDouble, d);
}

bool IHandler::RawNumber(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return ReportError(ErrorType::UnexpectedNumber, str);
}

bool IHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return ReportError(ErrorType::UnexpectedString, str);
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
