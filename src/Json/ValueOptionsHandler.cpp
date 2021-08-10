#include "Json/ValueOptionsHandler.h"
#include "Utils.h"
#include "ColorUtil.h"

ValueOptionsHandler::ValueOptionsHandler(
	ReaderHandler* master,
	ValueOptionsData* data,
	const std::string& modName,
	RE::TESForm* sourceForm,
	const std::string& scriptName) :
	IHandler{ master },
	_data{ data },
	_modName{ modName },
	_sourceForm{ sourceForm },
	_scriptName{ scriptName }
{
}

bool ValueOptionsHandler::Bool(bool b)
{
	switch (_state) {
	case State::DefaultValue:
		_data->DefaultValue = b ? 1.0f : 0.0f;
		_state = State::Main;
		return true;
	default:
		return IHandler::Bool(b);
	}
}

bool ValueOptionsHandler::Int(int i)
{
	switch (_state) {
	case State::Min:
		_data->Min = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::Max:
		_data->Max = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::Step:
		_data->Step = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::DefaultValue:
		_data->DefaultValue = static_cast<float>(i);
		_state = State::Main;
		return true;
	default:
		return IHandler::Int(i);
	}
}

bool ValueOptionsHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Min:
		_data->Min = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::Max:
		_data->Max = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::Step:
		_data->Step = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::DefaultValue:
		_data->DefaultValue = static_cast<float>(i);
		_state = State::Main;
		return true;
	default:
		return IHandler::Uint(i);
	}
}

bool ValueOptionsHandler::Double(double d)
{
	switch (_state) {
	case State::Min:
		_data->Min = static_cast<float>(d);
		_state = State::Main;
		return true;
	case State::Max:
		_data->Max = static_cast<float>(d);
		_state = State::Main;
		return true;
	case State::Step:
		_data->Step = static_cast<float>(d);
		_state = State::Main;
		return true;
	case State::DefaultValue:
		_data->DefaultValue = static_cast<float>(d);
		_state = State::Main;
		return true;
	default:
		return IHandler::Double(d);
	}
}

bool ValueOptionsHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Value:
		_data->Value = str;
		_state = State::Main;
		return true;
	case State::FormatString:
		_data->FormatString = str;
		_state = State::Main;
		return true;
	case State::OptionElem:
		_data->Options.push_back(str);
		return true;
	case State::ShortNameElem:
		_data->ShortNames.push_back(str);
		return true;
	case State::SourceType:
		_data->SourceType = str;
		_state = State::Main;
		return true;
	case State::SourceForm:
		_data->SourceForm = Utils::GetFormFromIdentifier(str);
		_state = State::Main;
		return true;
	case State::ScriptName:
		_data->ScriptName = str;
		_state = State::Main;
		return true;
	case State::PropertyName:
		_data->PropertyName = str;
		_state = State::Main;
		return true;
	case State::DefaultValue:
		if (strncmp(str, "{i}", strlen("{i}")) == 0) {
			std::istringstream ss{ str + 3 };
			std::int32_t value;
			ss >> value;
			_data->DefaultValue = static_cast<float>(value);
		}
		else if (strncmp(str, "{u}", strlen("{u}")) == 0) {
			std::istringstream ss{ str + 3 };
			std::uint32_t value;
			ss >> value;
			_data->DefaultValue = static_cast<float>(value);
		}
		else if (strncmp(str, "{b}", strlen("{b}")) == 0) {
			std::istringstream ss{ str + 3 };
			bool value;
			ss >> value;
			_data->DefaultValue = value ? 1.0f : 0.0f;
		}
		else if (strncmp(str, "{f}", strlen("{f}")) == 0) {
			std::istringstream ss{ str + 3 };
			float value;
			ss >> value;
			_data->DefaultValue = value;
		}
		else if (strncmp(str, "{r}", strlen("{r}")) == 0) {
			std::istringstream ss{ str + 3 };
			RE::Color value;
			ss >> value;
			_data->DefaultValue = static_cast<float>(PackARGB(value));
		}
		else if (strncmp(str, "{s}", strlen("{s}")) == 0) {
			_data->DefaultValueStr = str + 3;
		}
		else {
			_data->DefaultValueStr = str;
		}

		_state = State::Main;
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool ValueOptionsHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool ValueOptionsHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Main:
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
		else if (strcmp(str, "formatString") == 0) {
			_state = State::FormatString;
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
		else if (strcmp(str, "defaultValue") == 0) {
			_state = State::DefaultValue;
			return true;
		}
	}
	return IHandler::Key(str, length, copy);
}

bool ValueOptionsHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Main:
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
				return ReportError(ErrorType::InvalidValue, _data->SourceType);
			}

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

			propertyValue->PropertyName = _data->PropertyName;
			propertyValue->DefaultValue = _data->DefaultValue;

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
				return ReportError(ErrorType::InvalidValue, _data->SourceType);
			}

			modSetting->ModName = _modName;

			_data->ValueSource = modSetting;
		}
		else if (_data->SourceType == "GlobalValue"s) {
			auto globalValue = std::make_shared<GlobalValue>();
			if (_data->SourceForm && _data->SourceForm->GetFormType() == RE::FormType::Global) {
				globalValue->SourceForm = static_cast<RE::TESGlobal*>(_data->SourceForm);
			}

			globalValue->DefaultValue = _data->DefaultValue;

			_data->ValueSource = globalValue;
		}
		else if (!_data->SourceType.empty()) {
			return ReportError(ErrorType::InvalidValue, _data->SourceType);
		}

		_master->PopHandler();
		return true;
	default:
		return IHandler::EndObject(memberCount);
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
		return IHandler::StartArray();
	}
}

bool ValueOptionsHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::OptionElem:
		_state = State::Main;
		return true;
	case State::ShortNameElem:
		_state = State::Main;
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}
