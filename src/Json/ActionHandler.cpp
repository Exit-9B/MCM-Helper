#include "Json/ActionHandler.h"
#include "Utils.h"

ActionHandler::ActionHandler(
	std::shared_ptr<Action>* action,
	RE::TESForm* sourceForm,
	const std::string& scriptName) :
	_action{ action },
	_sourceForm{ sourceForm },
	_scriptName{ scriptName }
{
}

bool ActionHandler::Complete()
{
	return _state == State::End;
}

bool ActionHandler::Bool(bool b)
{
	switch (_state) {
	case State::Params:
		return _params->Bool(b);
	default:
		return false;
	}
}

bool ActionHandler::Int(int i)
{
	switch (_state) {
	case State::Params:
		return _params->Int(i);
	default:
		return false;
	}
}

bool ActionHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Params:
		return _params->Uint(i);
	default:
		return false;
	}
}

bool ActionHandler::Double(double d)
{
	switch (_state) {
	case State::Params:
		return _params->Double(d);
	default:
		return false;
	}
}

bool ActionHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Type:
		_data.Type = str;
		_state = State::Start;
		return true;
	case State::Form:
		_data.Form = Utils::GetFormFromIdentifier(str);
		_state = State::Start;
		return true;
	case State::Script:
		_data.Script = str;
		_state = State::Start;
		return true;
	case State::ScriptName:
		_data.ScriptName = str;
		_state = State::Start;
		return true;
	case State::Function:
		_data.Function = str;
		_state = State::Start;
		return true;
	case State::Params:
		return _params->String(str, length, copy);
	default:
		return false;
	}
}

bool ActionHandler::StartObject()
{
	switch (_state) {
	case State::End:
		_state = State::Start;
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
	case State::Start:
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
			_params = std::make_unique<ParamsHandler>(&_data.Params);
			_state = State::Params;
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
	case State::Start:
		if (_data.Type == "CallFunction") {
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
		else if (_data.Type == "CallGlobalFunction") {
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

		_state = State::End;
		return true;
	default:
		return false;
	}
}

bool ActionHandler::StartArray()
{
	switch (_state) {
	case State::Params:
		return _params->StartArray();
	default:
		return false;
	}
}

bool ActionHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Params:
	{
		bool paramsOK = _params->EndArray(elementCount);
		if (_params->Complete())
			_state = State::Start;
		return paramsOK;
	}
	default:
		return false;
	}
}