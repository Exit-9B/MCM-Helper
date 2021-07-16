#include "Json/PluginRequirementsHandler.h"

bool PluginRequirementsHandler::Complete()
{
	return _state == State::End;
}

bool PluginRequirementsHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Start:
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
	case State::End:
		_state = State::Start;
	default:
		return false;
	}
}

bool PluginRequirementsHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	switch (_state) {
	case State::Start:
		_state = State::End;
	default:
		return false;
	}
}
