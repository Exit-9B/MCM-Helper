#pragma once

#include "SettingStore.h"

class ValueSource
{
public:
	[[nodiscard]] virtual float GetValue() const = 0;
	virtual void SetValue(float a_value) = 0;
	[[nodiscard]] virtual float GetDefaultValue() const;
	void ResetToDefault();
};

class PropertyValue : public ValueSource
{
public:
	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
	float DefaultValue;

	[[nodiscard]] virtual float GetValue() const override;
	virtual void SetValue(float a_value) override;
	[[nodiscard]] virtual float GetDefaultValue() const override;

	[[nodiscard]] virtual float GetVariableValue(RE::BSScript::Variable& a_variable) const = 0;
	virtual void SetVariableValue(RE::BSScript::Variable& a_variable, float a_value) const = 0;
};

class PropertyValueBool : public PropertyValue
{
public:
	[[nodiscard]] float GetVariableValue(RE::BSScript::Variable& a_variable) const override;
	void SetVariableValue(RE::BSScript::Variable& a_variable, float a_value) const override;
};

class PropertyValueInt : public PropertyValue
{
public:
	[[nodiscard]] float GetVariableValue(RE::BSScript::Variable& a_variable) const override;
	void SetVariableValue(RE::BSScript::Variable& a_variable, float a_value) const override;
};

class PropertyValueFloat : public PropertyValue
{
public:
	[[nodiscard]] float GetVariableValue(RE::BSScript::Variable& a_variable) const override;
	void SetVariableValue(RE::BSScript::Variable& a_variable, float a_value) const override;
};

class ModSetting : public ValueSource
{
public:
	std::string ModName;
	std::string ID;
};

class ModSettingBool : public ModSetting
{
public:
	[[nodiscard]] float GetValue() const override;
	void SetValue(float a_value) override;
	[[nodiscard]] float GetDefaultValue() const override;
};

class ModSettingInt : public ModSetting
{
public:
	[[nodiscard]] float GetValue() const override;
	void SetValue(float a_value) override;
	[[nodiscard]] float GetDefaultValue() const override;
};

class ModSettingFloat : public ModSetting
{
public:
	[[nodiscard]] float GetValue() const override;
	void SetValue(float a_value) override;
	[[nodiscard]] float GetDefaultValue() const override;
};

class GlobalValue : public ValueSource
{
public:
	RE::TESGlobal* SourceForm;
	float DefaultValue;

	[[nodiscard]] float GetValue() const override;
	void SetValue(float a_value) override;
	[[nodiscard]] float GetDefaultValue() const override;
};
