#pragma once

#include "SettingStore.h"

class ValueSource
{
public:
	virtual auto GetValue() -> float = 0;
	virtual void SetValue(float a_value) = 0;
	virtual auto GetDefaultValue() -> float;
	void ResetToDefault();
};

class PropertyValue : public ValueSource
{
public:
	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
	float DefaultValue;

	virtual auto GetValue() -> float override;
	virtual void SetValue(float a_value) override;
	virtual auto GetDefaultValue() -> float override;

	virtual auto GetValue(RE::BSScript::Variable& a_variable) -> float = 0;
	virtual void SetValue(RE::BSScript::Variable& a_variable, float a_value) = 0;
};

class PropertyValueBool : public PropertyValue
{
public:
	auto GetValue(RE::BSScript::Variable& a_variable) -> float override;
	void SetValue(RE::BSScript::Variable& a_variable, float a_value) override;
};

class PropertyValueInt : public PropertyValue
{
public:
	auto GetValue(RE::BSScript::Variable& a_variable) -> float override;
	void SetValue(RE::BSScript::Variable& a_variable, float a_value) override;
};

class PropertyValueFloat : public PropertyValue
{
public:
	auto GetValue(RE::BSScript::Variable& a_variable) -> float override;
	void SetValue(RE::BSScript::Variable& a_variable, float a_value) override;
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
	auto GetValue() -> float override;
	void SetValue(float a_value) override;
	auto GetDefaultValue() -> float override;
};

class ModSettingInt : public ModSetting
{
public:
	auto GetValue() -> float override;
	void SetValue(float a_value) override;
	auto GetDefaultValue() -> float override;
};

class ModSettingFloat : public ModSetting
{
public:
	auto GetValue() -> float override;
	void SetValue(float a_value) override;
	auto GetDefaultValue() -> float override;
};

class GlobalValue : public ValueSource
{
public:
	RE::TESGlobal* SourceForm;
	float DefaultValue;

	auto GetValue() -> float override;
	void SetValue(float a_value) override;
	auto GetDefaultValue() -> float override;
};