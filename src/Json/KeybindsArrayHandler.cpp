#include "Json/KeybindsArrayHandler.h"
#include "Json/ActionHandler.h"

KeybindsArrayHandler::KeybindsArrayHandler(
	ReaderHandler* master,
	std::unordered_map<std::string, KeybindInfo>* modKeys) :
	IHandler{ master },
	_modKeys{ modKeys }
{
}

bool KeybindsArrayHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::ID:
		_id = str;
		return true;
	case State::Desc:
		_desc = str;
		return true;
	default:
		return false;
	}
}

bool KeybindsArrayHandler::StartObject()
{
	switch (_state) {
	case State::Main:
		_state = State::Keybind;
		return true;
	default:
		return false;
	}
}

bool KeybindsArrayHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Keybind:
		if (strcmp(str, "id") == 0) {
			_state = State::ID;
			return true;
		}
		else if (strcmp(str, "desc") == 0) {
			_state = State::Desc;
			return true;
		}
		else if (strcmp(str, "action") == 0) {
			_master->PushHandler<ActionHandler>(_master, std::addressof(_action));
			return true;
		}
		else {
			return false;
		}
	default:
		return false;
	}
}

bool KeybindsArrayHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Keybind:
		if (_id.empty())
			return false;

		if (auto sendEvent = std::dynamic_pointer_cast<SendEvent>(_action)) {
			sendEvent->Control = _id;
		}

		(*_modKeys)[_id] = KeybindInfo{
			.Keybind = Keybind{ .KeybindID = _id, },
			.KeybindDesc = _desc,
			.Action = _action,
		};

		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool KeybindsArrayHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool KeybindsArrayHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return false;
	}
}
