#include "Json/PluginRequirementsHandler.h"

PluginRequirementsHandler::PluginRequirementsHandler(ReaderHandler* master) : IHandler{ master }
{
}

bool PluginRequirementsHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Main:
	{
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		return dataHandler && dataHandler->LookupModByName(str);
	}
	default:
		return false;
	}
}

bool PluginRequirementsHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
	default:
		return false;
	}
}

bool PluginRequirementsHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
	default:
		return false;
	}
}
