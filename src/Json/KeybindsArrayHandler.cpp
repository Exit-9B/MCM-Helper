#include "Json/KeybindsArrayHandler.h"
#include "Json/ActionHandler.h"

KeybindsArrayHandler::KeybindsArrayHandler(
	ReaderHandler* master,
	std::unordered_map<std::string, KeybindInfo>* modKeys) :
	IHandler(master),
	_modKeys{ modKeys }
{
}

bool KeybindsArrayHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::ID:
		_id = str;
		_state = State::Keybind;
		return true;
	case State::Desc:
		_desc = str;
		_state = State::Keybind;
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool KeybindsArrayHandler::StartObject()
{
	switch (_state) {
	case State::Main:
		_state = State::Keybind;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool KeybindsArrayHandler::Key(const Ch* str, SizeType length, bool copy)
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
			_master->PushHandler<ActionHandler>(std::addressof(_action));
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool KeybindsArrayHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Keybind:
		if (_id.empty()) {
			return ReportError(ErrorType::MissingRequiredField, "id"sv);
		}

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
		return IHandler::EndObject(memberCount);
	}
}

bool KeybindsArrayHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartArray();
	}
}

bool KeybindsArrayHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}
