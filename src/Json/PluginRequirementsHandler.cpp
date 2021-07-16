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
	if (_state == State::Start) {
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		return dataHandler && dataHandler->LookupModByName(str);
	}

	return false;
}

bool PluginRequirementsHandler::StartArray()
{
	if (_state == State::End) {
		_state = State::Start;
		return true;
	}

	return false;
}

bool PluginRequirementsHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	if (_state == State::Start) {
		_state = State::End;
		return true;
	}

	return false;
}
