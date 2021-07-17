#include "Json/IHandler.h"

bool IHandler::Null()
{
	return false;
}

bool IHandler::Bool([[maybe_unused]] bool b)
{
	return false;
}

bool IHandler::Int([[maybe_unused]] int i)
{
	return false;
}

bool IHandler::Uint([[maybe_unused]] unsigned i)
{
	return false;
}

bool IHandler::Int64([[maybe_unused]] std::int64_t i)
{
	return false;
}

bool IHandler::Uint64([[maybe_unused]] std::uint64_t i)
{
	return false;
}

bool IHandler::Double([[maybe_unused]] double d)
{
	return false;
}

bool IHandler::RawNumber(
	[[maybe_unused]] const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return false;
}

bool IHandler::String(
	[[maybe_unused]] const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return false;
}

bool IHandler::StartObject()
{
	return false;
}

bool IHandler::Key(
	[[maybe_unused]] const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return false;
}

bool IHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	return false;
}

bool IHandler::StartArray()
{
	return false;
}

bool IHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	return false;
}
