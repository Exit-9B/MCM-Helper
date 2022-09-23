#pragma once

#include "Json/IHandler.h"

class UserKeybindsHandler : public IHandler
{
public:
	UserKeybindsHandler(ReaderHandler* master);

	bool Uint(unsigned i) override;
	bool StartObject() override;
	bool Key(const Ch* str, SizeType length, bool copy) override;
	bool EndObject(SizeType memberCount) override;

private:
	enum class State
	{
		Init,
		Main,
		Version,
	};

	State _state = State::Init;
};
