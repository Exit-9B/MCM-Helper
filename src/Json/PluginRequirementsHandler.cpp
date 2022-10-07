#include "Json/PluginRequirementsHandler.h"

PluginRequirementsHandler::PluginRequirementsHandler(ReaderHandler* master) : IHandler(master)
{
}

bool PluginRequirementsHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Main:
	{
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		return dataHandler && dataHandler->LookupModByName(str);
	}
	default:
		return IHandler::String(str, length, copy);
	}
}

bool PluginRequirementsHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartArray();
	}
}

bool PluginRequirementsHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}
