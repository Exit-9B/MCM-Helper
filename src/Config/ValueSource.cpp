#include "Config/ValueSource.h"
#include "ConfigPageCache.h"
#include "ColorUtil.h"
#include "Script/ScriptObject.h"

float ValueSource::GetDefaultValue() const
{
	return GetValue();
}

void ValueSource::ResetToDefault()
{
	SetValue(GetDefaultValue());
}

float PropertyValue::GetValue() const
{
	const auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	const auto variable = script ? script->GetProperty(PropertyName) : nullptr;
	if (variable) {
		return GetVariableValue(*variable);
	}

	return 0.0f;
}

void PropertyValue::SetValue(float a_value)
{
	const auto script = ScriptObject::FromForm(SourceForm, ScriptName);
	const auto variable = script ? script->GetProperty(PropertyName) : nullptr;
	if (variable) {
		SetVariableValue(*variable, a_value);
	}
}

float PropertyValue::GetDefaultValue() const
{
	return DefaultValue;
}

float PropertyValueBool::GetVariableValue(RE::BSScript::Variable& a_variable) const
{
	if (a_variable.IsBool()) {
		return a_variable.GetBool() ? 1.0f : 0.0f;
	}

	return 0.0f;
}

void PropertyValueBool::SetVariableValue(RE::BSScript::Variable& a_variable, float a_value) const
{
	a_variable.SetBool(a_value);
}

float PropertyValueInt::GetVariableValue(RE::BSScript::Variable& a_variable) const
{
	if (a_variable.IsInt()) {
		return static_cast<float>(a_variable.GetSInt());
	}

	return 0.0f;
}

void PropertyValueInt::SetVariableValue(RE::BSScript::Variable& a_variable, float a_value) const
{
	a_variable.SetSInt(static_cast<std::int32_t>(a_value));
}

float PropertyValueFloat::GetVariableValue(RE::BSScript::Variable& a_variable) const
{
	if (a_variable.IsFloat()) {
		return a_variable.GetFloat();
	}

	return 0.0f;
}

void PropertyValueFloat::SetVariableValue(RE::BSScript::Variable& a_variable, float a_value) const
{
	a_variable.SetFloat(a_value);
}

float ModSettingBool::GetValue() const
{
	const auto boolValue = SettingStore::GetInstance().GetModSettingBool(ModName, ID);
	return boolValue ? 1.0f : 0.0f;
}

void ModSettingBool::SetValue(float a_value)
{
	SettingStore::GetInstance().SetModSettingBool(ModName, ID, a_value);
}

float ModSettingBool::GetDefaultValue() const
{
	const auto setting = SettingStore::GetInstance().GetDefaultSetting(ModName, ID);
	if (setting) {
		return setting->data.b ? 1.0f : 0.0f;
	}
	else {
		return GetValue();
	}
}

float ModSettingInt::GetValue() const
{
	const auto intValue = SettingStore::GetInstance().GetModSettingInt(ModName, ID);
	return static_cast<float>(intValue);
}

void ModSettingInt::SetValue(float a_value)
{
	SettingStore::GetInstance().SetModSettingInt(ModName, ID, static_cast<std::int32_t>(a_value));
}

float ModSettingInt::GetDefaultValue() const
{
	const auto setting = SettingStore::GetInstance().GetDefaultSetting(ModName, ID);
	if (setting) {
		switch (setting->GetType()) {
		case RE::Setting::Type::kUnsignedInteger:
			return static_cast<float>(setting->data.u);
		case RE::Setting::Type::kColor:
			return static_cast<float>(PackARGB(setting->data.r));
		default:
			return static_cast<float>(setting->data.i);
		}
	}
	else {
		return GetValue();
	}
}

float ModSettingFloat::GetValue() const
{
	return SettingStore::GetInstance().GetModSettingFloat(ModName, ID);
}

void ModSettingFloat::SetValue(float a_value)
{
	SettingStore::GetInstance().SetModSettingFloat(ModName, ID, a_value);
}

float ModSettingFloat::GetDefaultValue() const
{
	const auto setting = SettingStore::GetInstance().GetDefaultSetting(ModName, ID);
	if (setting) {
		return setting->data.f;
	}
	else {
		return GetValue();
	}
}

float GlobalValue::GetValue() const
{
	return SourceForm ? SourceForm->value : 0.0f;
}

void GlobalValue::SetValue(float a_value)
{
	if (SourceForm) {
		SourceForm->value = a_value;
	}
}

float GlobalValue::GetDefaultValue() const
{
	return DefaultValue;
}
