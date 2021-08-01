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
		return i <= PLUGIN_VERSION;
	default:
		return false;
	}
}

bool UserKeybindsHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool UserKeybindsHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
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
			return false;
		}
	default:
		return false;
	}
}

bool UserKeybindsHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Main:
		return true;
	default:
		return false;
	}
}