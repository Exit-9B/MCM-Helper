#include "Config/Control.h"

#include "Script/SkyUI.h"
#include "SettingStore.h"
#include "KeybindManager.h"
#include "ConfigPageCache.h"
#include "ColorUtil.h"
#include "Translation.h"
#include "InputMap.h"

void Control::Refresh(
	[[maybe_unused]] const ScriptObjectPtr& a_configScript,
	[[maybe_unused]] std::int32_t a_optionID)
{
}

void Control::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm);
	}
}

void Control::ResetToDefault()
{
	// base behavior is to do nothing
}

auto Control::GetInfoText() -> std::string
{
	std::string text = Help;
	std::string value = Translation::ScaleformTranslate(GetValueString());

	constexpr auto valueToken = "{value}"sv;
	constexpr auto length = valueToken.length();

	auto pos = text.rfind(valueToken);
	while (pos != std::string::npos) {
		text = text.replace(pos, length, value);
		pos = text.rfind(valueToken, pos);
	}

	return text;
}

auto Control::GetValueString() -> std::string
{
	return "{value}"s;
}

auto Control::GetFlags() -> SkyUI::Flags
{
	if (GroupCondition && !GroupCondition->GetIsActive()) {
		switch (GroupBehavior) {
		case Behavior::Disable:
			return SkyUI::Flags::Disable;
		case Behavior::Hide:
			return SkyUI::Flags::Disable | SkyUI::Flags::Hide;
		}
	}

	return SkyUI::Flags::None;
}

auto Control::GetDesiredBehavior() -> Behavior
{
	if (GroupCondition && !GroupCondition->GetIsActive()) {
		return GroupBehavior;
	}

	return Behavior::Normal;
}

void Control::RefreshFlags(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	if (GroupCondition > 0) {
		if (GetDesiredBehavior() == Behavior::Skip)
			return;

		SkyUI::Config::SetOptionFlags(a_configScript, a_optionID, GetFlags());
	}
}

auto EmptyControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	if (Position >= 0) {
		SkyUI::Config::SetCursorPosition(a_configScript, Position);
	}
	return SkyUI::Config::AddEmptyOption(a_configScript);
}

auto HeaderControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	return SkyUI::Config::AddHeaderOption(a_configScript, Text, GetFlags());
}

void HeaderControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	RefreshFlags(a_configScript, a_optionID);
}

auto TextControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto value = GetValue();
	return SkyUI::Config::AddTextOption(a_configScript, Text, value, GetFlags());
}

void TextControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto value = GetValue();
	SkyUI::Config::SetTextOptionValue(a_configScript, a_optionID, value);
	RefreshFlags(a_configScript, a_optionID);
}

void TextControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, GetValue());
	}
}

void TextControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto TextControl::GetValueString() -> std::string
{
	return GetValue();
}

auto TextControl::GetValue() -> std::string
{
	return ValueSource ? ValueSource->GetValue() : Value;
}

auto ToggleControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto checked = GetValue();
	return SkyUI::Config::AddToggleOption(a_configScript, Text, checked, GetFlags());
}

void ToggleControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto checked = GetValue();
	SkyUI::Config::SetToggleOptionValue(a_configScript, a_optionID, checked);
	RefreshFlags(a_configScript, a_optionID);
}

void ToggleControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, GetValue());
	}
}

void ToggleControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto ToggleControl::GetValueString() -> std::string
{
	return GetValue() ? "1"s : "0"s;
}

auto ToggleControl::GetValue() -> bool
{
	return ValueSource && ValueSource->GetValue() != 0.0f;
}

auto SliderControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto value = GetValue();
	return SkyUI::Config::AddSliderOption(a_configScript, Text, value, FormatString, GetFlags());
}

void SliderControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto value = GetValue();
	SkyUI::Config::SetSliderOptionValue(a_configScript, a_optionID, value, FormatString);
	RefreshFlags(a_configScript, a_optionID);
}

void SliderControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, GetValue());
	}
}

void SliderControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto SliderControl::GetValueString() -> std::string
{
	auto stepStr = std::to_string(Step);
	auto precision = stepStr.find_last_not_of('0') - stepStr.find('.');
	auto decimalOffset = precision > 0 ? 1 : 0;

	auto valueStr = std::to_string(GetValue());
	auto newLength = valueStr.find('.') + precision + decimalOffset;
	valueStr = valueStr.substr(0, newLength);

	return valueStr;
}

auto SliderControl::GetValue() -> float
{
	return ValueSource ? ValueSource->GetValue() : 0.0f;
}

auto StepperControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto text = GetText();
	return SkyUI::Config::AddTextOption(a_configScript, Text, text, GetFlags());
}

void StepperControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto text = GetText();
	SkyUI::Config::SetTextOptionValue(a_configScript, a_optionID, text);
	RefreshFlags(a_configScript, a_optionID);
}

void StepperControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, GetValue());
	}
}

void StepperControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto StepperControl::GetValueString() -> std::string
{
	return std::to_string(GetValue());
}

auto StepperControl::GetValue() -> std::int32_t
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

auto StepperControl::GetText() -> std::string
{
	auto value = GetValue();
	if (value >= 0 && value < Options.size()) {
		return Options[value];
	}

	return ""s;
}

auto MenuControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto value = GetShortText();
	return SkyUI::Config::AddMenuOption(a_configScript, Text, value, GetFlags());
}

void MenuControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto value = GetShortText();
	SkyUI::Config::SetMenuOptionValue(a_configScript, a_optionID, value);
	RefreshFlags(a_configScript, a_optionID);
}

void MenuControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, GetValue());
	}
}

void MenuControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto MenuControl::GetValueString() -> std::string
{
	return GetValue();
}

auto MenuControl::GetValue() -> std::string
{
	return ValueSource ? ValueSource->GetValue() : ""s;
}

auto MenuControl::GetDefaultValue() -> std::string
{
	return ValueSource ? ValueSource->GetDefaultValue() : ""s;
}

auto MenuControl::GetShortText() -> std::string
{
	auto& configPageCache = ConfigPageCache::GetInstance();
	auto options = configPageCache.GetMenuOptions(this);
	auto shortNames = configPageCache.GetMenuShortNames(this);

	if (options.size() != shortNames.size())
		return GetValue();

	auto value = GetValue();
	auto item = std::find(options.begin(), options.end(), value);
	auto index = static_cast<std::int32_t>(item - options.begin());

	return shortNames[index];
}

auto EnumControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto text = GetShortText();
	return SkyUI::Config::AddMenuOption(a_configScript, Text, text, GetFlags());
}

void EnumControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto text = GetShortText();
	SkyUI::Config::SetMenuOptionValue(a_configScript, a_optionID, text);
	RefreshFlags(a_configScript, a_optionID);
}

void EnumControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, GetValue());
	}
}

void EnumControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto EnumControl::GetValueString() -> std::string
{
	return std::to_string(GetValue());
}

auto EnumControl::GetValue() -> std::int32_t
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

auto EnumControl::GetShortText() -> std::string
{
	auto& configPageCache = ConfigPageCache::GetInstance();
	auto options = configPageCache.GetMenuOptions(this);
	auto shortNames = configPageCache.GetMenuShortNames(this);

	auto index = GetValue();

	if (index < 0 || index >= options.size()) {
		return ""s;
	}

	if (options.size() != shortNames.size()) {
		return options[index];
	}

	return shortNames[index];
}

auto ColorControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto value = GetColor();
	return SkyUI::Config::AddColorOption(a_configScript, Text, value, GetFlags());
}

void ColorControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto value = GetColor();
	SkyUI::Config::SetColorOptionValue(a_configScript, a_optionID, value);
	RefreshFlags(a_configScript, a_optionID);
}

void ColorControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, static_cast<std::int32_t>(GetColor()));
	}
}

void ColorControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto ColorControl::GetValueString() -> std::string
{
	return std::to_string(UnpackARGB(GetColor()));
}

auto ColorControl::GetColor() -> std::uint32_t
{
	return ValueSource ? static_cast<std::uint32_t>(ValueSource->GetValue()) : 0;
}

auto KeyMapControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto keyCode = GetKeyCode();
	return SkyUI::Config::AddKeyMapOption(a_configScript, Text, keyCode, GetFlags());
}

void KeyMapControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto keyCode = GetKeyCode();
	SkyUI::Config::SetKeyMapOptionValue(a_configScript, a_optionID, keyCode);
	RefreshFlags(a_configScript, a_optionID);
}

void KeyMapControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, static_cast<std::int32_t>(GetKeyCode()));
	}
}

void KeyMapControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto KeyMapControl::GetInfoText() -> std::string
{
	auto text = Control::GetInfoText();
	if (!text.empty()) {
		return text;
	}

	if (!ValueSource && !ID.empty()) {
		auto& keybindManager = KeybindManager::GetInstance();
		const auto& desc = keybindManager.GetKeybind(ModName, ID).KeybindDesc;
		return desc;
	}

	return ""s;
}

auto KeyMapControl::GetValueString() -> std::string
{
	return InputMap::GetKeyName(GetKeyCode());
}

auto KeyMapControl::GetKeyCode() -> std::uint32_t
{
	if (ValueSource) {
		return static_cast<std::uint32_t>(ValueSource->GetValue());
	}
	else {
		return KeybindManager::GetInstance().GetRegisteredKey(ModName, ID);
	}
}

auto KeyMapControl::GetDescription() -> std::string
{
	if (!ValueSource) {
		auto& keybindManager = KeybindManager::GetInstance();
		const auto& desc = keybindManager.GetKeybind(ModName, ID).KeybindDesc;
		if (!desc.empty()) {
			return desc;
		}
	}

	return Text;
}

auto InputControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto value = GetValue();
	return SkyUI::Config::AddInputOption(a_configScript, Text, value, GetFlags());
}

void InputControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	RefreshFlags(a_configScript, a_optionID);
}

void InputControl::InvokeAction(VM* a_vm)
{
	if (Action) {
		Action->Invoke(a_vm, GetValue());
	}
}

void InputControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

auto InputControl::GetValueString() -> std::string
{
	return GetValue();
}

auto InputControl::GetValue() -> std::string
{
	return ValueSource ? ValueSource->GetValue() : ""s;
}

auto HiddenToggleControl::Add([[maybe_unused]] const ScriptObjectPtr& a_configScript)
	-> std::int32_t
{
	return -1;
}

void HiddenToggleControl::Refresh(
	[[maybe_unused]] const ScriptObjectPtr& a_configScript,
	[[maybe_unused]] std::int32_t a_optionID)
{
}

auto ErrorControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	return SkyUI::Config::AddTextOption(a_configScript, "Error", "Click to view");
}
