#include "ValueSource.h"

#include "ConfigPageCache.h"

float ValueSource::GetDefaultValue()
{
	return GetValue();
}

float PropertyValue::GetValue()
{
	auto& configPageCache = ConfigPageCache::GetInstance();
	auto variable = configPageCache.GetPropertyVariable(SourceForm, ScriptName, PropertyName);
	if (variable)
	{
		return GetValue(*variable);
	}

	return 0.0f;
}

float PropertyValueBool::GetValue(RE::BSScript::Variable& a_variable)
{
	if (a_variable.IsBool())
	{
		return a_variable.GetBool() ? 1.0f : 0.0f;
	}

	return 0.0f;
}

float PropertyValueInt::GetValue(RE::BSScript::Variable& a_variable)
{
	if (a_variable.IsInt())
	{
		return static_cast<float>(a_variable.GetSInt());
	}

	return 0.0f;
}

float PropertyValueFloat::GetValue(RE::BSScript::Variable& a_variable)
{
	if (a_variable.IsFloat())
	{
		return a_variable.GetFloat();
	}

	return 0.0f;
}

float ModSettingBool::GetValue()
{
	auto boolValue = SettingStore::GetInstance().GetModSettingBool(ModName, ID);
	return boolValue ? 1.0f : 0.0f;
}

float ModSettingInt::GetValue()
{
	auto intValue = SettingStore::GetInstance().GetModSettingInt(ModName, ID);
	return static_cast<float>(intValue);
}

float ModSettingFloat::GetValue()
{
	return SettingStore::GetInstance().GetModSettingFloat(ModName, ID);
}

float GlobalValue::GetValue()
{
	return SourceForm ? SourceForm->value : 0.0f;
}

void PropertyValue::SetValue(float a_value)
{
	auto& configPageCache = ConfigPageCache::GetInstance();
	auto variable = configPageCache.GetPropertyVariable(SourceForm, ScriptName, PropertyName);
	if (variable)
	{
		SetValue(*variable, a_value);
	}
}

void PropertyValueBool::SetValue(RE::BSScript::Variable& a_variable, float a_value)
{
	a_variable.SetBool(a_value);
}

void PropertyValueInt::SetValue(RE::BSScript::Variable& a_variable, float a_value)
{
	a_variable.SetSInt(static_cast<std::int32_t>(a_value));
}

void PropertyValueFloat::SetValue(RE::BSScript::Variable& a_variable, float a_value)
{
	a_variable.SetFloat(a_value);
}

void ModSettingBool::SetValue(float a_value)
{
	SettingStore::GetInstance().SetModSettingBool(ModName, ID, a_value);
}

void ModSettingInt::SetValue(float a_value)
{
	SettingStore::GetInstance().SetModSettingInt(ModName, ID, static_cast<std::int32_t>(a_value));
}

void ModSettingFloat::SetValue(float a_value)
{
	SettingStore::GetInstance().SetModSettingFloat(ModName, ID, a_value);
}

void GlobalValue::SetValue(float a_value)
{
	if (SourceForm)
	{
		SourceForm->value = a_value;
	}
}