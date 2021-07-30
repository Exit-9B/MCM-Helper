#include "Config/TextSource.h"
#include "Script/ScriptObject.h"
#include "SettingStore.h"

auto PropertyString::GetValue() -> std::string
{
	auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	auto variable = script ? script->GetProperty(PropertyName) : nullptr;
	if (variable && variable->IsString()) {
		return std::string{ variable->GetString() };
	}

	return ""s;
}

void PropertyString::SetValue(std::string_view a_value)
{
	auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	auto variable = script ? script->GetProperty(PropertyName) : nullptr;

	if (variable) {
		variable->SetString(a_value);
	}
}

auto PropertyString::GetDefaultValue() -> std::string
{
	return DefaultValue;
}

void PropertyString::ResetToDefault()
{
	auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	auto variable = script ? script->GetProperty(PropertyName) : nullptr;
	if (variable) {
		variable->SetString(DefaultValue);
	}
}

auto ModSettingString::GetValue() -> std::string
{
	return SettingStore::GetInstance().GetModSettingString(ModName, ID);
}

void ModSettingString::SetValue(std::string_view a_value)
{
	SettingStore::GetInstance().SetModSettingString(ModName, ID, a_value);
}

auto ModSettingString::GetDefaultValue() -> std::string
{
	auto setting = SettingStore::GetInstance().GetDefaultSetting(ModName, ID);
	return setting ? setting->GetString() : ""s;
}

void ModSettingString::ResetToDefault()
{
	SettingStore::GetInstance().ResetToDefault(ModName, ID);
}