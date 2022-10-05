#include "Json/UserKeybindsHandler.h"
#include "Json/UserKeybindsArrayHandler.h"

UserKeybindsHandler::UserKeybindsHandler(ReaderHandler* master) : IHandler{ master }
{
}

bool UserKeybindsHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Version:
		_state = State::Main;
		if (i > MCM_VERSION_RELEASE) {
			return ReportError(
				"Keybind registrations require {} plugin version: {}"sv,
				Plugin::NAME,
				i);
		}
		return true;
	default:
		return IHandler::Uint(i);
	}
}

bool UserKeybindsHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool UserKeybindsHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Main:
		if (strcmp(str, "version") == 0) {
			_state = State::Version;
			return true;
		}
		else if (strcmp(str, "keybinds") == 0) {
			_master->PushHandler<UserKeybindsArrayHandler>();
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool UserKeybindsHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Main:
		return true;
	default:
		return IHandler::EndObject(memberCount);
	}
}
