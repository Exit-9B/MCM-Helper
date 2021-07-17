#include "Json/ActionHandler.h"
#include "Json/ParamsHandler.h"
#include "Utils.h"

ActionHandler::ActionHandler(
	ReaderHandler* master,
	std::shared_ptr<Action>* action,
	RE::TESForm* sourceForm,
	const std::string& scriptName) :
	IHandler{ master },
	_action{ action },
	_sourceForm{ sourceForm },
	_scriptName{ scriptName }
{
}

bool ActionHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Type:
		_data.Type = str;
		_state = State::Main;
		return true;
	case State::Form:
		_data.Form = Utils::GetFormFromIdentifier(str);
		_state = State::Main;
		return true;
	case State::Script:
		_data.Script = str;
		_state = State::Main;
		return true;
	case State::ScriptName:
		_data.ScriptName = str;
		_state = State::Main;
		return true;
	case State::Function:
		_data.Function = str;
		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool ActionHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool ActionHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Main:
		if (strcmp(str, "type") == 0) {
			_state = State::Type;
			return true;
		}
		else if (strcmp(str, "form") == 0) {
			_state = State::Form;
			return true;
		}
		else if (strcmp(str, "scriptName") == 0) {
			_state = State::ScriptName;
			return true;
		}
		else if (strcmp(str, "script") == 0) {
			_state = State::Script;
			return true;
		}
		else if (strcmp(str, "function") == 0) {
			_state = State::Function;
			return true;
		}
		else if (strcmp(str, "params") == 0) {
			_master->PushHandler<ParamsHandler>(_master, &_data.Params);
			return true;
		}
		else {
			return false;
		}
	default:
		return false;
	}
}

bool ActionHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Main:
		if (_data.Type == "CallFunction"s) {
			auto callFunction = std::make_shared<CallFunction>();
			*_action = callFunction;
			if (_data.Form) {
				callFunction->Form = _data.Form;
				callFunction->ScriptName = _data.ScriptName;
			}
			else {
				callFunction->Form = _sourceForm;
				callFunction->ScriptName =
					!_data.Script.empty() ? _data.Script
					: _scriptName;
			}
		}
		else if (_data.Type == "CallGlobalFunction"s) {
			auto callGlobalFunction = std::make_shared<CallGlobalFunction>();
			*_action = callGlobalFunction;
			callGlobalFunction->ScriptName =
				!_data.Script.empty() ? _data.Script
				: _scriptName;
		}
		else {
			return false;
		}

		(*_action)->Params = _data.Params;
		(*_action)->Function = _data.Function;

		_master->PopHandler();
		return true;
	default:
		return false;
	}
}