#include "Json/UserKeybindsArrayHandler.h"

UserKeybindsArrayHandler::UserKeybindsArrayHandler(ReaderHandler* master) : IHandler{ master }
{
}

bool UserKeybindsArrayHandler::Int(int i)
{
	switch (_state) {
	case State::Keycode:
		_keyCode = -1;
		_state = State::Keybind;
		return true;
	default:
		return IHandler::Int(i);
	}
}

bool UserKeybindsArrayHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Keycode:
		if (i <= static_cast<unsigned>(std::numeric_limits<std::int32_t>::max())) {
			_keyCode = static_cast<std::int32_t>(i);
		}
		else {
			_keyCode = -1;
		}
		_state = State::Keybind;
		return true;
	default:
		return IHandler::Uint(i);
	}
}

bool UserKeybindsArrayHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::ModName:
		_modName = str;
		_state = State::Keybind;
		return true;
	case State::ID:
		_id = str;
		_state = State::Keybind;
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool UserKeybindsArrayHandler::StartObject()
{
	switch (_state) {
	case State::Main:
		_state = State::Keybind;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool UserKeybindsArrayHandler::Key(const Ch* str, SizeType length, bool copy)
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
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool UserKeybindsArrayHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Keybind:
		KeybindManager::GetInstance().Register(
			static_cast<std::uint32_t>(_keyCode),
			_modName,
			_id);
		_state = State::Main;
		return true;
	default:
		return IHandler::EndObject(memberCount);
	}
}

bool UserKeybindsArrayHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartArray();
	}
}

bool UserKeybindsArrayHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}
