#pragma once

#include <rapidjson/reader.h>

class PluginRequirementsHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	bool Complete();

	bool String(const Ch* str, SizeType length, bool copy);
	bool StartArray();
	bool EndArray(SizeType elementCount);

private:
	enum class State
	{
		End,
		Start,
	};

	State _state = State::End;
};
