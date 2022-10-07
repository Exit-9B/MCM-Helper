#include "Config/TextSource.h"
#include "Script/ScriptObject.h"
#include "SettingStore.h"

std::string PropertyString::GetValue() const
{
	const auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	const auto variable = script ? script->GetProperty(PropertyName) : nullptr;
	if (variable && variable->IsString()) {
		return std::string(variable->GetString());
	}

	return ""s;
}

void PropertyString::SetValue(std::string_view a_value)
{
	const auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	const auto variable = script ? script->GetProperty(PropertyName) : nullptr;

	if (variable) {
		variable->SetString(a_value);
	}
}

std::string PropertyString::GetDefaultValue() const
{
	return DefaultValue;
}

void PropertyString::ResetToDefault()
{
	const auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	const auto variable = script ? script->GetProperty(PropertyName) : nullptr;
	if (variable) {
		variable->SetString(DefaultValue);
	}
}

std::string ModSettingString::GetValue() const
{
	auto value = SettingStore::GetInstance().GetModSettingString(ModName, ID);
	return value ? value : ""s;
}

void ModSettingString::SetValue(std::string_view a_value)
{
	SettingStore::GetInstance().SetModSettingString(ModName, ID, a_value);
}

std::string ModSettingString::GetDefaultValue() const
{
	const auto setting = SettingStore::GetInstance().GetDefaultSetting(ModName, ID);
	return setting ? setting->GetString() : ""s;
}

void ModSettingString::ResetToDefault()
{
	SettingStore::GetInstance().ResetToDefault(ModName, ID);
}
