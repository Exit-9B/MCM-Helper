#pragma once

#include "SettingStore.h"
#include "Utils.h"

class ValueSource
{
public:
	virtual float GetValue() = 0;
	virtual void SetValue(float a_value) = 0;
	virtual float GetDefaultValue();
};

class PropertyValue : public ValueSource
{
public:
	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;

	virtual float GetValue() override;
	virtual void SetValue(float a_value) override;
	virtual float GetValue(RE::BSScript::Variable& a_variable) = 0;
	virtual void SetValue(RE::BSScript::Variable& a_variable, float a_value) = 0;
};

class PropertyValueBool : public PropertyValue
{
public:
	virtual float GetValue(RE::BSScript::Variable& a_variable) override;
	virtual void SetValue(RE::BSScript::Variable& a_variable, float a_value) override;
};

class PropertyValueInt : public PropertyValue
{
public:
	virtual float GetValue(RE::BSScript::Variable& a_variable) override;
	virtual void SetValue(RE::BSScript::Variable& a_variable, float a_value) override;
};

class PropertyValueFloat : public PropertyValue
{
public:
	virtual float GetValue(RE::BSScript::Variable& a_variable) override;
	virtual void SetValue(RE::BSScript::Variable& a_variable, float a_value) override;
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
	virtual float GetValue() override;
	virtual void SetValue(float a_value) override;
};

class ModSettingInt : public ModSetting
{
public:
	virtual float GetValue() override;
	virtual void SetValue(float a_value) override;
};

class ModSettingFloat : public ModSetting
{
public:
	virtual float GetValue() override;
	virtual void SetValue(float a_value) override;
};

class GlobalValue : public ValueSource
{
public:
	RE::TESGlobal* SourceForm;

	virtual float GetValue() override;
	virtual void SetValue(float a_value) override;
};