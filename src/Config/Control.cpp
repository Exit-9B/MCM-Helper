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

VMAwaitable Control::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm);
}

void Control::ResetToDefault()
{
	// base behavior is to do nothing
}

std::string Control::GetInfoText() const
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

std::string Control::GetValueString() const
{
	return "{value}"s;
}

SkyUI::Flags Control::GetFlags() const
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

auto Control::GetDesiredBehavior() const -> Behavior
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

std::int32_t EmptyControl::Add(const ScriptObjectPtr& a_configScript)
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	if (Position >= 0) {
		SkyUI::Config::SetCursorPosition(a_configScript, Position);
	}
	return SkyUI::Config::AddEmptyOption(a_configScript);
}

std::int32_t HeaderControl::Add(const ScriptObjectPtr& a_configScript)
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	return SkyUI::Config::AddHeaderOption(a_configScript, Text, GetFlags());
}

void HeaderControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	RefreshFlags(a_configScript, a_optionID);
}

std::int32_t TextControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable TextControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, GetValue());
}

void TextControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string TextControl::GetValueString() const
{
	return GetValue();
}

std::string TextControl::GetValue() const
{
	return ValueSource ? ValueSource->GetValue() : Value;
}

std::int32_t ToggleControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable ToggleControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, GetValue());
}

void ToggleControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string ToggleControl::GetValueString() const
{
	return GetValue() ? "1"s : "0"s;
}

bool ToggleControl::GetValue() const
{
	return ValueSource && ValueSource->GetValue() != 0.0f;
}

std::int32_t SliderControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable SliderControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, GetValue());
}

void SliderControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string SliderControl::GetValueString() const
{
	auto stepStr = std::to_string(Step);
	auto precision = stepStr.find_last_not_of('0') - stepStr.find('.');
	auto decimalOffset = precision > 0 ? 1 : 0;

	auto valueStr = std::to_string(GetValue());
	auto newLength = valueStr.find('.') + precision + decimalOffset;
	valueStr = valueStr.substr(0, newLength);

	return valueStr;
}

float SliderControl::GetValue() const
{
	return ValueSource ? ValueSource->GetValue() : 0.0f;
}

std::int32_t StepperControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable StepperControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, GetValue());
}

void StepperControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string StepperControl::GetValueString() const
{
	return std::to_string(GetValue());
}

std::int32_t StepperControl::GetValue() const
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

std::string StepperControl::GetText() const
{
	auto value = GetValue();
	if (value >= 0 && value < Options.size()) {
		return Options[value];
	}

	return ""s;
}

std::int32_t MenuControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable MenuControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, GetValue());
}

void MenuControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string MenuControl::GetValueString() const
{
	return GetValue();
}

std::string MenuControl::GetValue() const
{
	return ValueSource ? ValueSource->GetValue() : ""s;
}

std::string MenuControl::GetDefaultValue() const
{
	return ValueSource ? ValueSource->GetDefaultValue() : ""s;
}

std::string MenuControl::GetShortText() const
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

std::int32_t EnumControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable EnumControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, GetValue());
}

void EnumControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string EnumControl::GetValueString() const
{
	return std::to_string(GetValue());
}

std::int32_t EnumControl::GetValue() const
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

std::string EnumControl::GetShortText() const
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

std::int32_t ColorControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable ColorControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, static_cast<std::int32_t>(GetColor()));
}

void ColorControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string ColorControl::GetValueString() const
{
	return std::to_string(UnpackARGB(GetColor()));
}

std::uint32_t ColorControl::GetColor() const
{
	return ValueSource ? static_cast<std::uint32_t>(ValueSource->GetValue()) : 0;
}

std::int32_t KeyMapControl::Add(const ScriptObjectPtr& a_configScript)
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

VMAwaitable KeyMapControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, static_cast<std::int32_t>(GetKeyCode()));
}

void KeyMapControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string KeyMapControl::GetInfoText() const
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

std::string KeyMapControl::GetValueString() const
{
	return InputMap::GetKeyName(GetKeyCode());
}

SkyUI::Flags KeyMapControl::GetFlags() const
{
	auto flags = Control::GetFlags();
	return flags == SkyUI::Flags::None ? SkyUI::Flags::WithUnmap : flags;
}

std::uint32_t KeyMapControl::GetKeyCode() const
{
	if (ValueSource) {
		return static_cast<std::uint32_t>(ValueSource->GetValue());
	}
	else {
		return KeybindManager::GetInstance().GetRegisteredKey(ModName, ID);
	}
}

std::string KeyMapControl::GetDescription() const
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

std::int32_t InputControl::Add(const ScriptObjectPtr& a_configScript)
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	auto value = GetValue();
	return SkyUI::Config::AddInputOption(a_configScript, Text, value, GetFlags());
}

void InputControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto value = GetValue();
	SkyUI::Config::SetInputOptionValue(a_configScript, a_optionID, value);
	RefreshFlags(a_configScript, a_optionID);
}

VMAwaitable InputControl::InvokeAction(VM* a_vm)
{
	if (!Action)
		return {};

	return Action->Invoke(a_vm, GetValue());
}

void InputControl::ResetToDefault()
{
	if (ValueSource) {
		ValueSource->ResetToDefault();
	}
}

std::string InputControl::GetValueString() const
{
	return GetValue();
}

std::string InputControl::GetValue() const
{
	return ValueSource ? ValueSource->GetValue() : ""s;
}

std::int32_t HiddenToggleControl::Add([[maybe_unused]] const ScriptObjectPtr& a_configScript)
{
	return -1;
}

void HiddenToggleControl::Refresh(
	[[maybe_unused]] const ScriptObjectPtr& a_configScript,
	[[maybe_unused]] std::int32_t a_optionID)
{
}

std::int32_t ErrorControl::Add(const ScriptObjectPtr& a_configScript)
{
	return SkyUI::Config::AddTextOption(
		a_configScript,
		"Error loading config"sv,
		"Click to view"sv);
}
