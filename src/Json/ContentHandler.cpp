#include "Json/ContentHandler.h"
#include "Json/GroupConditionHandler.h"
#include "Json/ActionHandler.h"
#include "FormUtil.h"

ContentHandler::ContentHandler(
	ReaderHandler* master,
	PageLayout* pageLayout,
	const ScriptObjectPtr& script) :
	IHandler(master),
	_pageLayout{ pageLayout }
{
	_form = static_cast<RE::TESForm*>(script->Resolve(0));
	_modName = FormUtil::GetModName(_form);
	_scriptName = script->GetTypeInfo()->GetName();
}

bool ContentHandler::Bool(bool b)
{
	switch (_state) {
	case State::IgnoreConflicts:
		_data.IgnoreConflicts = b;
		_state = State::Control;
		return true;
	default:
		return IHandler::Bool(b);
	}
}

bool ContentHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Position:
		_data.Position = static_cast<std::int32_t>(i);
		_state = State::Control;
		return true;
	case State::GroupControl:
		_data.GroupControl = static_cast<std::uint32_t>(i);
		_state = State::Control;
		return true;
	default:
		return IHandler::Uint(i);
	}
}

bool ContentHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::ID:
		_data.ID = str;
		_state = State::Control;
		return true;
	case State::Text:
		_data.Text = str;
		_state = State::Control;
		return true;
	case State::Help:
		_data.Help = str;
		_state = State::Control;
		return true;
	case State::Type:
		_data.Type = str;
		_state = State::Control;
		return true;
	case State::GroupBehavior:
		if (strcmp(str, "disable") == 0) {
			_data.GroupBehavior = Control::Behavior::Disable;
			_state = State::Control;
			return true;
		}
		else if (strcmp(str, "hide") == 0) {
			_data.GroupBehavior = Control::Behavior::Hide;
			_state = State::Control;
			return true;
		}
		else if (strcmp(str, "skip") == 0) {
			_data.GroupBehavior = Control::Behavior::Skip;
			_state = State::Control;
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidValue, str);
		}
	case State::FormatString:
		// Kept for backwards compatibility with v1.0.3 - 4
		_data.ValueOptions.FormatString = str;
		_state = State::Control;
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool ContentHandler::StartObject()
{
	switch (_state) {
	case State::Main:
		_state = State::Control;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool ContentHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Control:
		if (strcmp(str, "id") == 0) {
			_state = State::ID;
			return true;
		}
		else if (strcmp(str, "position") == 0) {
			_state = State::Position;
			return true;
		}
		else if (strcmp(str, "text") == 0) {
			_state = State::Text;
			return true;
		}
		else if (strcmp(str, "help") == 0) {
			_state = State::Help;
			return true;
		}
		else if (strcmp(str, "type") == 0) {
			_state = State::Type;
			return true;
		}
		else if (strcmp(str, "groupCondition") == 0) {
			_data.GroupCondition = std::make_shared<GroupConditionTree>();
			_master->PushHandler<GroupConditionHandler>(_data.GroupCondition);
			return true;
		}
		else if (strcmp(str, "groupBehavior") == 0) {
			_state = State::GroupBehavior;
			return true;
		}
		else if (strcmp(str, "groupControl") == 0) {
			_state = State::GroupControl;
			return true;
		}
		else if (strcmp(str, "formatString") == 0) {
			// Kept for backwards compatibility with v1.0.3 - 4
			_state = State::FormatString;
			return true;
		}
		else if (strcmp(str, "ignoreConflicts") == 0) {
			_state = State::IgnoreConflicts;
			return true;
		}
		else if (strcmp(str, "action") == 0) {
			_master->PushHandler<ActionHandler>(std::addressof(_data.Action), _form, _scriptName);
			return true;
		}
		else if (strcmp(str, "valueOptions") == 0) {
			_master->PushHandler<ValueOptionsHandler>(
				std::addressof(_data.ValueOptions),
				_modName,
				_form,
				_scriptName);
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool ContentHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Control:
	{
		if (const auto modSetting = std::dynamic_pointer_cast<ModSetting>(
				_data.ValueOptions.ValueSource)) {
			modSetting->ID = _data.ID;
		}

		std::shared_ptr<TextSource> textSource;
		if (!_data.ValueOptions.ValueSource) {
			if (!_data.ValueOptions.PropertyName.empty()) {
				const auto propertyString = std::make_shared<PropertyString>();
				if (_data.ValueOptions.SourceForm) {
					propertyString->SourceForm = _data.ValueOptions.SourceForm;
					propertyString->ScriptName = _data.ValueOptions.ScriptName;
				}
				else {
					propertyString->SourceForm = _form;
					propertyString->ScriptName = !_data.ValueOptions.ScriptName.empty()
						? _data.ValueOptions.ScriptName
						: _scriptName;
				}

				propertyString->PropertyName = _data.ValueOptions.PropertyName;
				propertyString->DefaultValue = _data.ValueOptions.DefaultValueStr;
				textSource = propertyString;
			}
			else if (!_data.ID.empty()) {
				const auto modSettingString = std::make_shared<ModSettingString>();
				modSettingString->ID = _data.ID;
				modSettingString->ModName = _modName;
				textSource = modSettingString;
			}
		}

		std::shared_ptr<Control> control;
		if (_data.Type == "empty"s) {
			control = std::make_shared<EmptyControl>();
		}
		else if (_data.Type == "header"s) {
			control = std::make_shared<HeaderControl>();
		}
		else if (_data.Type == "text"s) {
			const auto textOption = std::make_shared<TextControl>();
			textOption->Value = _data.ValueOptions.Value;
			textOption->ValueSource = textSource;
			control = textOption;
		}
		else if (_data.Type == "toggle"s) {
			const auto toggleOption = std::make_shared<ToggleControl>();
			toggleOption->GroupControl = _data.GroupControl;
			toggleOption->ValueSource = _data.ValueOptions.ValueSource;
			control = toggleOption;
		}
		else if (_data.Type == "hiddenToggle"s) {
			const auto hiddenToggleOption = std::make_shared<HiddenToggleControl>();
			hiddenToggleOption->GroupControl = _data.GroupControl;
			hiddenToggleOption->ValueSource = _data.ValueOptions.ValueSource;
			control = hiddenToggleOption;
		}
		else if (_data.Type == "slider"s) {
			const auto sliderOption = std::make_shared<SliderControl>();
			sliderOption->Min = _data.ValueOptions.Min;
			sliderOption->Max = _data.ValueOptions.Max;
			sliderOption->Step = _data.ValueOptions.Step;
			sliderOption->FormatString = _data.ValueOptions.FormatString;
			sliderOption->ValueSource = _data.ValueOptions.ValueSource;
			control = sliderOption;
		}
		else if (_data.Type == "stepper"s) {
			const auto stepperOption = std::make_shared<StepperControl>();
			stepperOption->Options = _data.ValueOptions.Options;
			stepperOption->ValueSource = _data.ValueOptions.ValueSource;
			control = stepperOption;
		}
		else if (_data.Type == "menu"s) {
			const auto menuOption = std::make_shared<MenuControl>();
			menuOption->Options = _data.ValueOptions.Options;
			menuOption->ShortNames = _data.ValueOptions.ShortNames;
			menuOption->ValueSource = textSource;
			control = menuOption;
		}
		else if (_data.Type == "enum"s) {
			const auto enumOption = std::make_shared<EnumControl>();
			enumOption->Options = _data.ValueOptions.Options;
			enumOption->ShortNames = _data.ValueOptions.ShortNames;
			enumOption->ValueSource = _data.ValueOptions.ValueSource;
			control = enumOption;
		}
		else if (_data.Type == "color"s) {
			const auto colorOption = std::make_shared<ColorControl>();
			colorOption->ValueSource = _data.ValueOptions.ValueSource;
			control = colorOption;
		}
		else if (_data.Type == "keymap"s) {
			const auto keymapOption = std::make_shared<KeyMapControl>();
			keymapOption->ModName = _modName;
			keymapOption->IgnoreConflicts = _data.IgnoreConflicts;
			keymapOption->ValueSource = _data.ValueOptions.ValueSource;
			control = keymapOption;
		}
		else if (_data.Type == "input"s) {
			const auto inputOption = std::make_shared<InputControl>();
			inputOption->ValueSource = textSource;
			control = inputOption;
		}

		if (control) {
			control->Position = _data.Position;
			control->ID = _data.ID;
			control->Text = _data.Text;
			control->Help = _data.Help;
			control->GroupCondition = _data.GroupCondition;
			control->GroupBehavior = _data.GroupBehavior;

			if (_data.Action) {
				const auto function = std::dynamic_pointer_cast<Function>(_data.Action);
				if (!function) {
					return ReportError("Unsupported action type"sv);
				}

				control->Action = function;
			}

			_pageLayout->Controls.push_back(control);

			if (const auto toggle = std::dynamic_pointer_cast<ToggleControl>(control)) {
				if (toggle->GroupControl > 0) {
					_pageLayout->GroupControls[toggle->GroupControl] = toggle;
				}
			}
		}
		else {
			return ReportError(ErrorType::InvalidValue, _data.Type);
		}

		_data = ControlData{};
		_state = State::Main;
		return true;
	}
	default:
		return IHandler::EndObject(memberCount);
	}
}

bool ContentHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartArray();
	}
}

bool ContentHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}
