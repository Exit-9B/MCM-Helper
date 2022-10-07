#include "Json/ActionHandler.h"
#include "Json/ParamsHandler.h"
#include "FormUtil.h"

ActionHandler::ActionHandler(
	ReaderHandler* master,
	std::shared_ptr<Action>* action,
	RE::TESForm* sourceForm,
	const std::string& scriptName) :
	IHandler(master),
	_action{ action },
	_sourceForm{ sourceForm },
	_scriptName{ scriptName }
{
}

bool ActionHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Type:
		_data.Type = str;
		_state = State::Main;
		return true;
	case State::Form:
		_data.Form = FormUtil::GetFormFromIdentifier(str);
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
	case State::Command:
		_data.Command = str;
		_state = State::Main;
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool ActionHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool ActionHandler::Key(const Ch* str, SizeType length, bool copy)
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
		else if (strcmp(str, "command") == 0) {
			_state = State::Command;
			return true;
		}
		else if (strcmp(str, "params") == 0) {
			_master->PushHandler<ParamsHandler>(std::addressof(_data.Params));
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool ActionHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Main:
		if (_data.Type == "CallFunction"s) {
			if (_data.Function.empty()) {
				return ReportError(ErrorType::MissingRequiredField, "function"sv);
			}

			auto callFunction = std::make_shared<CallFunction>();
			*_action = callFunction;
			if (_data.Form) {
				callFunction->Form = _data.Form;
				callFunction->ScriptName = _data.ScriptName;
			}
			else {
				callFunction->Form = _sourceForm;
				callFunction->ScriptName = !_data.Script.empty() ? _data.Script : _scriptName;
			}
			callFunction->Params = _data.Params;
			callFunction->Function = _data.Function;
		}
		else if (_data.Type == "CallGlobalFunction"s) {
			if (_data.Function.empty()) {
				return ReportError(ErrorType::MissingRequiredField, "function"sv);
			}

			auto callGlobalFunction = std::make_shared<CallGlobalFunction>();
			*_action = callGlobalFunction;
			callGlobalFunction->ScriptName = !_data.Script.empty() ? _data.Script : _scriptName;
			callGlobalFunction->Params = _data.Params;
			callGlobalFunction->Function = _data.Function;
		}
		else if (_data.Type == "RunConsoleCommand"s) {
			if (_data.Command.empty()) {
				return ReportError(ErrorType::MissingRequiredField, "command"sv);
			}

			auto runConsoleCommand = std::make_shared<RunConsoleCommand>();
			*_action = runConsoleCommand;
			runConsoleCommand->Command = _data.Command;
		}
		else if (_data.Type == "SendEvent"s) {
			if (!_data.Form) {
				return ReportError(ErrorType::MissingRequiredField, "form"sv);
			}

			auto sendEvent = std::make_shared<SendEvent>();
			*_action = sendEvent;
			sendEvent->Form = _data.Form;
			sendEvent->ScriptName = _data.ScriptName;
		}
		else if (!_data.Type.empty()) {
			return ReportError(ErrorType::InvalidValue, _data.Type);
		}
		else {
			return ReportError(ErrorType::MissingRequiredField, "type"sv);
		}

		_master->PopHandler();
		return true;
	default:
		return IHandler::EndObject(memberCount);
	}
}
