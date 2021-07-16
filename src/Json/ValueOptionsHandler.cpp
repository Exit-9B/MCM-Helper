#include "Json/ValueOptionsHandler.h"
#include "Utils.h"

ValueOptionsHandler::ValueOptionsHandler(
	ValueOptionsData* data,
	const std::string& modName,
	const std::string& id,
	RE::TESForm* sourceForm,
	const std::string& scriptName) :
	_data{ data },
	_modName{ modName },
	_id{ id },
	_sourceForm{ sourceForm },
	_scriptName{ scriptName }
{
}

bool ValueOptionsHandler::Complete()
{
	return _state == State::End;
}

bool ValueOptionsHandler::Int(int i)
{
	switch (_state) {
	case State::Min:
		_data->Min = static_cast<float>(i);
		_state = State::Start;
		return true;
	case State::Max:
		_data->Max = static_cast<float>(i);
		_state = State::Start;
		return true;
	case State::Step:
		_data->Step = static_cast<float>(i);
		_state = State::Start;
		return true;
	default:
		return false;
	}
}

bool ValueOptionsHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Min:
		_data->Min = static_cast<float>(i);
		_state = State::Start;
		return true;
	case State::Max:
		_data->Max = static_cast<float>(i);
		_state = State::Start;
		return true;
	case State::Step:
		_data->Step = static_cast<float>(i);
		_state = State::Start;
		return true;
	default:
		return false;
	}
}

bool ValueOptionsHandler::Double(double d)
{
	switch (_state) {
	case State::Min:
		_data->Min = static_cast<float>(d);
		_state = State::Start;
		return true;
	case State::Max:
		_data->Max = static_cast<float>(d);
		_state = State::Start;
		return true;
	case State::Step:
		_data->Step = static_cast<float>(d);
		_state = State::Start;
		return true;
	default:
		return false;
	}
}

bool ValueOptionsHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Value:
		_data->Value = str;
		_state = State::Start;
		return true;
	case State::OptionElem:
		_data->Options.push_back(str);
		return true;
	case State::ShortNameElem:
		_data->ShortNames.push_back(str);
		return true;
	case State::SourceType:
		_data->SourceType = str;
		_state = State::Start;
		return true;
	case State::SourceForm:
		_data->SourceForm = Utils::GetFormFromIdentifier(str);
		_state = State::Start;
		return true;
	case State::ScriptName:
		_data->ScriptName = str;
		_state = State::Start;
		return true;
	case State::PropertyName:
		_data->PropertyName = str;
		_state = State::Start;
		return true;
	default:
		return false;
	}
}

bool ValueOptionsHandler::StartObject()
{
	switch (_state) {
	case State::End:
		_state = State::Start;
		return true;
	default:
		return false;
	}
}

bool ValueOptionsHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Start:
		if (strcmp(str, "min") == 0) {
			_state = State::Min;
			return true;
		}
		else if (strcmp(str, "max") == 0) {
			_state = State::Max;
			return true;
		}
		else if (strcmp(str, "step") == 0) {
			_state = State::Step;
			return true;
		}
		else if (strcmp(str, "value") == 0) {
			_state = State::Value;
			return true;
		}
		else if (strcmp(str, "options") == 0) {
			_state = State::Options;
			return true;
		}
		else if (strcmp(str, "shortNames") == 0) {
			_state = State::ShortNames;
			return true;
		}
		else if (strcmp(str, "sourceType") == 0) {
			_state = State::SourceType;
			return true;
		}
		else if (strcmp(str, "sourceForm") == 0) {
			_state = State::SourceForm;
			return true;
		}
		else if (strcmp(str, "scriptName") == 0) {
			_state = State::ScriptName;
			return true;
		}
		else if (strcmp(str, "propertyName") == 0) {
			_state = State::PropertyName;
			return true;
		}
	}
	return false;
}

bool ValueOptionsHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Start:
		if (_data->SourceType.rfind("PropertyValue", 0) == 0) {
			std::shared_ptr<PropertyValue> propertyValue;
			if (_data->SourceType == "PropertyValueBool"s) {
				propertyValue = std::make_shared<PropertyValueBool>();
			}
			else if (_data->SourceType == "PropertyValueInt"s) {
				propertyValue = std::make_shared<PropertyValueInt>();
			}
			else if (_data->SourceType == "PropertyValueFloat"s) {
				propertyValue = std::make_shared<PropertyValueFloat>();
			}
			else {
				return false;
			}

			propertyValue->PropertyName = _data->PropertyName;
			if (_data->SourceForm) {
				propertyValue->SourceForm = _data->SourceForm;
				propertyValue->ScriptName = _data->ScriptName;
			}
			else {
				propertyValue->SourceForm = _sourceForm;
				propertyValue->ScriptName =
					!_data->ScriptName.empty() ? _data->ScriptName
					: _scriptName;
			}

			_data->ValueSource = propertyValue;
		}
		else if (_data->SourceType.rfind("ModSetting", 0) == 0) {
			std::shared_ptr<ModSetting> modSetting;
			if (_data->SourceType == "ModSettingBool"s) {
				modSetting = std::make_shared<ModSettingBool>();
			}
			else if (_data->SourceType == "ModSettingInt"s) {
				modSetting = std::make_shared<ModSettingInt>();
			}
			else if (_data->SourceType == "ModSettingFloat"s) {
				modSetting = std::make_shared<ModSettingFloat>();
			}
			else {
				return false;
			}

			modSetting->ModName = _modName;
			modSetting->ID = _id;

			_data->ValueSource = modSetting;
		}
		else if (_data->SourceType == "GlobalValue"s) {
			std::shared_ptr<GlobalValue> globalValue;
			globalValue->SourceForm = skyrim_cast<RE::TESGlobal*>(_data->SourceForm);

			_data->ValueSource = globalValue;
		}
		else if (!_data->SourceType.empty()) {
			return false;
		}

		_state = State::End;
		return true;
	default:
		return false;
	}
}

bool ValueOptionsHandler::StartArray()
{
	switch (_state) {
	case State::Options:
		_state = State::OptionElem;
		return true;
	case State::ShortNames:
		_state = State::ShortNameElem;
		return true;
	default:
		return false;
	}
}

bool ValueOptionsHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	switch (_state) {
	case State::OptionElem:
		_state = State::Start;
		return true;
	case State::ShortNameElem:
		_state = State::Start;
		return true;
	default:
		return false;
	}
}