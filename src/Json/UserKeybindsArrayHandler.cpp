#include "Json/UserKeybindsArrayHandler.h"

UserKeybindsArrayHandler::UserKeybindsArrayHandler(ReaderHandler* master) : IHandler{ master }
{
}

bool UserKeybindsArrayHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Keycode:
		_keyCode = static_cast<std::int32_t>(i);
		return true;
	default:
		return false;
	}
}

bool UserKeybindsArrayHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::ModName:
		_modName = str;
		return true;
	case State::ID:
		_id = str;
		return true;
	default:
		return false;
	}
}

bool UserKeybindsArrayHandler::StartObject()
{
	switch (_state) {
	case State::Main:
		_state = State::Keybind;
		return true;
	default:
		return false;
	}
}

bool UserKeybindsArrayHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Keybind:
		if (strcmp(str, "keycode") == 0) {
			_state = State::Keycode;
			return true;
		}
		else if (strcmp(str, "modName") == 0) {
			_state = State::ModName;
			return true;
		}
		else if (strcmp(str, "id") == 0) {
			_state = State::ID;
			return true;
		}
		else {
			return false;
		}
	default:
		return false;
	}
}

bool UserKeybindsArrayHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Keybind:
		KeybindManager::GetInstance().Register(_keyCode, _modName, _id);
		return true;
	default:
		return false;
	}
}

bool UserKeybindsArrayHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool UserKeybindsArrayHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	switch (_state) {
	default:
		return false;
	}
}
