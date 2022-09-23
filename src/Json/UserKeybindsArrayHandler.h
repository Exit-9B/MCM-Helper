#pragma once

#include "Json/IHandler.h"
#include "KeybindManager.h"

class UserKeybindsArrayHandler : public IHandler
{
public:
	UserKeybindsArrayHandler(ReaderHandler* master);

	bool Int(int i) override;
	bool Uint(unsigned i) override;
	bool String(const Ch* str, SizeType length, bool copy) override;
	bool StartObject() override;
	bool Key(const Ch* str, SizeType length, bool copy) override;
	bool EndObject(SizeType memberCount) override;
	bool StartArray() override;
	bool EndArray(SizeType elementCount) override;

private:
	enum class State
	{
		Init,
		Main,
		Keybind,
		Keycode,
		ModName,
		ID,
	};

	State _state = State::Init;

	std::int32_t _keyCode = -1;
	std::string _modName;
	std::string _id;
};
