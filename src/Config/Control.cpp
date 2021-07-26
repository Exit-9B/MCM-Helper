#include "Config/Control.h"

#include "Script/SkyUI.h"
#include "SettingStore.h"
#include "KeybindManager.h"
#include "ConfigPageCache.h"

void Control::Refresh(
	[[maybe_unused]] const ScriptObjectPtr& a_configScript,
	[[maybe_unused]] std::int32_t a_optionID)
{
}

void Control::InvokeAction(VM* a_vm)
{
	if (Action)
	{
		Action->Invoke(a_vm);
	}
}

void Control::ResetToDefault()
{
	// base behavior is to do nothing
}

auto Control::GetInfoText() -> std::string
{
	return Help;
}

auto Control::GetFlags() -> SkyUI::Flags
{
	if (GroupCondition && !GroupCondition->GetIsActive())
	{
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
	if (GroupCondition && !GroupCondition->GetIsActive())
	{
		return GroupBehavior;
	}

	return Behavior::Normal;
}

void Control::RefreshFlags(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	if (GroupCondition > 0)
	{
		if (GetDesiredBehavior() == Behavior::Skip)
			return;

		SkyUI::Config::SetOptionFlags(a_configScript, a_optionID, GetFlags());
	}
}

auto EmptyControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (GetDesiredBehavior() == Behavior::Skip)
		return -1;

	if (Position >= 0)
	{
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
	if (Action)
	{
		Action->Invoke(a_vm, GetValue());
	}
}

void TextControl::ResetToDefault()
{
	if (!PropertyName.empty())
	{
		auto variable = Utils::GetScriptProperty(SourceForm, ScriptName, PropertyName);
		if (variable)
		{
			variable->SetString(DefaultValue);
		}
	}
	else if (!ID.empty())
	{
		SettingStore::GetInstance().ResetToDefault(ModName, ID);
	}
}

auto TextControl::GetValue() -> std::string
{
	if (!PropertyName.empty())
	{
		auto variable = Utils::GetScriptProperty(SourceForm, ScriptName, PropertyName);
		if (variable && variable->IsString())
		{
			return std::string{ variable->GetString() };
		}

		return ""s;
	}

	if (!ID.empty())
	{
		return SettingStore::GetInstance().GetModSettingString(ModName, ID);
	}

	return Value;
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
	if (Action)
	{
		Action->Invoke(a_vm, GetValue());
	}
}

void ToggleControl::ResetToDefault()
{
	ValueSource->ResetToDefault();
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
	if (Action)
	{
		Action->Invoke(a_vm, GetValue());
	}
}

void SliderControl::ResetToDefault()
{
	ValueSource->ResetToDefault();
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
	if (Action)
	{
		Action->Invoke(a_vm, GetValue());
	}
}

void StepperControl::ResetToDefault()
{
	ValueSource->ResetToDefault();
}

auto StepperControl::GetValue() -> std::int32_t
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

auto StepperControl::GetText() -> std::string
{
	auto value = GetValue();
	if (value >= 0 && value < Options.size())
	{
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
	if (Action)
	{
		Action->Invoke(a_vm, GetValue());
	}
}

void MenuControl::ResetToDefault()
{
	if (!PropertyName.empty())
	{
		auto variable = Utils::GetScriptProperty(SourceForm, ScriptName, PropertyName);
		if (variable)
		{
			variable->SetString(DefaultValue);
		}
	}
	else if (!ID.empty())
	{
		SettingStore::GetInstance().ResetToDefault(ModName, ID);
	}
}

auto MenuControl::GetValue() -> std::string
{
	if (!PropertyName.empty())
	{
		auto variable = Utils::GetScriptProperty(SourceForm, ScriptName, PropertyName);
		if (variable && variable->IsString())
		{
			return std::string{ variable->GetString() };
		}

		return ""s;
	}

	if (!ID.empty())
	{
		return SettingStore::GetInstance().GetModSettingString(ModName, ID);
	}

	return ""s;
}

auto MenuControl::GetDefaultValue() -> std::string
{
	if (!ID.empty())
	{
		auto setting = SettingStore::GetInstance().GetDefaultSetting(ModName, ID);
		if (setting && setting->GetType() == RE::Setting::Type::kString)
		{
			return setting->GetString();
		}
	}

	return ""s;
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
	if (Action)
	{
		Action->Invoke(a_vm, GetValue());
	}
}

void EnumControl::ResetToDefault()
{
	ValueSource->ResetToDefault();
}

auto EnumControl::GetValue() -> std::int32_t
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

auto EnumControl::GetShortText() -> std::string
{
	auto value = GetValue();
	if (value >= 0 && value < Options.size())
	{
		if (ShortNames.size() == Options.size())
			return ShortNames[value];
		else
			return Options[value];
	}

	return ""s;
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
	if (Action)
	{
		Action->Invoke(a_vm, static_cast<std::int32_t>(GetColor()));
	}
}

void ColorControl::ResetToDefault()
{
	ValueSource->ResetToDefault();
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
	if (Action)
	{
		Action->Invoke(a_vm, GetKeyCode());
	}
}

void KeyMapControl::ResetToDefault()
{
	ValueSource->ResetToDefault();
}

auto KeyMapControl::GetInfoText() -> std::string
{
	if (!ValueSource && !ID.empty())
	{
		auto& keybindManager = KeybindManager::GetInstance();
		const auto& desc = keybindManager.GetKeybind(ModName, ID).KeybindDesc;
		if (!desc.empty())
		{
			return desc;
		}
	}

	return Help;
}

auto KeyMapControl::GetKeyCode() -> std::int32_t
{
	if (ValueSource)
	{
		return static_cast<std::int32_t>(ValueSource->GetValue());
	}
	else
	{
		return KeybindManager::GetInstance().GetRegisteredKey(ModName, ID);
	}
}

auto KeyMapControl::GetDescription() -> std::string
{
	if (!ValueSource)
	{
		auto& keybindManager = KeybindManager::GetInstance();
		const auto& desc = keybindManager.GetKeybind(ModName, ID).KeybindDesc;
		if (!desc.empty())
		{
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
	if (Action)
	{
		Action->Invoke(a_vm, GetValue());
	}
}

void InputControl::ResetToDefault()
{
	if (!PropertyName.empty())
	{
		auto variable = Utils::GetScriptProperty(SourceForm, ScriptName, PropertyName);
		if (variable)
		{
			variable->SetString(DefaultValue);
		}
	}
	else if (!ID.empty())
	{
		SettingStore::GetInstance().ResetToDefault(ModName, ID);
	}
}

auto InputControl::GetValue() -> std::string
{
	if (!PropertyName.empty())
	{
		auto variable = Utils::GetScriptProperty(SourceForm, ScriptName, PropertyName);
		if (variable && variable->IsString())
		{
			return std::string{ variable->GetString() };
		}

		return ""s;
	}

	if (!ID.empty())
	{
		return SettingStore::GetInstance().GetModSettingString(ModName, ID);
	}

	return ""s;
}

auto HiddenToggleControl::Add(
	[[maybe_unused]] const ScriptObjectPtr& a_configScript)
	-> std::int32_t
{
	return -1;
}

void HiddenToggleControl::Refresh(
	[[maybe_unused]] const ScriptObjectPtr& a_configScript,
	[[maybe_unused]] std::int32_t a_optionID)
{
}