#pragma once

#include "Json/IHandler.h"

class ParamsHandler final : public IHandler
{
public:
	ParamsHandler(ReaderHandler* master, std::vector<std::string>* params);

	bool Bool(bool b) override;
	bool Int(int i) override;
	bool Uint(unsigned i) override;
	bool Double(double d) override;
	bool String(const Ch* str, SizeType length, bool copy) override;
	bool StartArray() override;
	bool EndArray(SizeType elementCount) override;

private:
	enum class State
	{
		Init,
		Main,
	};

	State _state = State::Init;

	std::vector<std::string>* _params;
};