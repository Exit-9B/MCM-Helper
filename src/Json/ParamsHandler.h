#pragma once

#include <rapidjson/reader.h>

class ParamsHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	ParamsHandler(std::vector<std::string>* params);
	bool Complete();

	bool Bool(bool b);
	bool Int(int i);
	bool Uint(unsigned i);
	bool Double(double d);
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
	std::vector<std::string>* _params;
};