#include "Control.h"

#include "SkyUI.h"
#include "SettingStore.h"
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

auto Control::GetFlags() -> SkyUI::Flags
{
	return ConfigPageCache::GetInstance().GetGroupFlags(GroupCondition);
}

void Control::RefreshFlags(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	if (GroupCondition > 0)
	{
		SkyUI::Config::SetOptionFlags(a_configScript, a_optionID, GetFlags());
	}
}

auto EmptyControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	if (Position >= 0)
	{
		SkyUI::Config::SetCursorPosition(a_configScript, Position);
	}
	return SkyUI::Config::AddEmptyOption(a_configScript);
}

auto HeaderControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	return SkyUI::Config::AddHeaderOption(a_configScript, Text, GetFlags());
}

void HeaderControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	RefreshFlags(a_configScript, a_optionID);
}

auto TextControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
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
		Action->InvokeString(a_vm, GetValue());
	}
}

auto TextControl::GetValue() -> std::string
{
	if (!PropertyName.empty())
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		auto variable = configPageCache.GetPropertyVariable(SourceForm, ScriptName, PropertyName);
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
		Action->InvokeBool(a_vm, GetValue());
	}
}

auto ToggleControl::GetValue() -> bool
{
	return ValueSource && ValueSource->GetValue() != 0.0f;
}

auto SliderControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
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
		Action->InvokeFloat(a_vm, GetValue());
	}
}

auto SliderControl::GetValue() -> float
{
	return ValueSource ? ValueSource->GetValue() : 0.0f;
}

auto MenuControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	auto value = GetValue();
	return SkyUI::Config::AddMenuOption(a_configScript, Text, value, GetFlags());
}

void MenuControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto value = GetValue();
	SkyUI::Config::SetMenuOptionValue(a_configScript, a_optionID, value);
	RefreshFlags(a_configScript, a_optionID);
}

void MenuControl::InvokeAction(VM* a_vm)
{
	if (Action)
	{
		Action->InvokeString(a_vm, GetValue());
	}
}

auto MenuControl::GetValue() -> std::string
{
	if (!PropertyName.empty())
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		auto variable = configPageCache.GetPropertyVariable(SourceForm, ScriptName, PropertyName);
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

auto EnumControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
	auto text = GetText();
	return SkyUI::Config::AddMenuOption(a_configScript, Text, text, GetFlags());
}

void EnumControl::Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID)
{
	auto text = GetText();
	SkyUI::Config::SetMenuOptionValue(a_configScript, a_optionID, text);
	RefreshFlags(a_configScript, a_optionID);
}

void EnumControl::InvokeAction(VM* a_vm)
{
	if (Action)
	{
		Action->InvokeInt(a_vm, GetValue());
	}
}

auto EnumControl::GetValue() -> std::int32_t
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

auto EnumControl::GetText() -> std::string
{
	auto value = GetValue();
	if (value >= 0 && value < Options.size())
	{
		return Options[value];
	}

	return ""s;
}

auto ColorControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
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
		Action->InvokeInt(a_vm, static_cast<std::int32_t>(GetColor()));
	}
}

auto ColorControl::GetColor() -> std::uint32_t
{
	return ValueSource ? static_cast<std::uint32_t>(ValueSource->GetValue()) : 0;
}

auto KeyMapControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
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
		Action->InvokeInt(a_vm, GetKeyCode());
	}
}

auto KeyMapControl::GetKeyCode() -> std::int32_t
{
	return ValueSource ? static_cast<std::int32_t>(ValueSource->GetValue()) : 0;
}

auto InputControl::Add(const ScriptObjectPtr& a_configScript) -> std::int32_t
{
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
		Action->InvokeString(a_vm, GetValue());
	}
}

auto InputControl::GetValue() -> std::string
{
	if (!PropertyName.empty())
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		auto variable = configPageCache.GetPropertyVariable(SourceForm, ScriptName, PropertyName);
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