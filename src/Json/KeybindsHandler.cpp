#include "Json/KeybindsHandler.h"
#include "Json/KeybindsArrayHandler.h"

KeybindsHandler::KeybindsHandler(ReaderHandler* master, const std::string& modName) :
	IHandler{ master },
	_modName{ modName }
{
}

bool KeybindsHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::ModName:
		_hasModName = true;
		_state = State::Main;
		return str == _modName;
	default:
		return false;
	}
}

bool KeybindsHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool KeybindsHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Main:
		if (strcmp(str, "modName") == 0) {
			_state = State::ModName;
			return true;
		}
		else if (strcmp(str, "keybinds") == 0) {
			_master->PushHandler<KeybindsArrayHandler>(_master, std::addressof(_modKeys));
			return true;
		}
		else {
			return false;
		}
	default:
		return false;
	}
}

bool KeybindsHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Main:
	{
		if (!_hasModName)
			return false;

		auto& keybindManager = KeybindManager::GetInstance();
		for (auto& [id, info] : _modKeys) {
			info.ModName = _modName;
			keybindManager.AddKeybind(_modName, id, info);
		}

		return true;
	}
	default:
		return false;
	}
}