#include "Json/KeybindsHandler.h"
#include "Json/KeybindsArrayHandler.h"

KeybindsHandler::KeybindsHandler(ReaderHandler* master, const std::string& modName) :
	IHandler{ master },
	_modName{ modName }
{
}

bool KeybindsHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Schema:
		// Ignored
		_state = State::Main;
		return true;
	case State::ModName:
		_hasModName = true;
		_state = State::Main;
		if (str != _modName) {
			return ReportError("modName: \"{}\" did not match plugin"sv, str);
		}
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool KeybindsHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool KeybindsHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Main:
		if (strcmp(str, "$schema") == 0) {
			_state = State::Schema;
			return true;
		}
		else if (strcmp(str, "modName") == 0) {
			_state = State::ModName;
			return true;
		}
		else if (strcmp(str, "keybinds") == 0) {
			_master->PushHandler<KeybindsArrayHandler>(std::addressof(_modKeys));
			return true;
		}
		else {
			return IHandler::ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool KeybindsHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Main:
	{
		if (!_hasModName) {
			return ReportError(ErrorType::MissingRequiredField, "modName"sv);
		}

		auto& keybindManager = KeybindManager::GetInstance();
		for (auto& [id, info] : _modKeys) {
			info.Keybind.ModName = _modName;
			keybindManager.AddKeybind(_modName, id, info);
		}

		return true;
	}
	default:
		return IHandler::EndObject(memberCount);
	}
}
