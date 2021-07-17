#pragma once

#include "Json/IHandler.h"

class ParamsHandler : public IHandler
{
public:
	ParamsHandler(ReaderHandler* master, std::vector<std::string>* params);

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
		Init,
		Main,
	};

	State _state = State::Init;

	std::vector<std::string>* _params;
};