#pragma once

#include "Config/ValueSource.h"
#include "Config/GroupConditionTree.h"
#include "Config/Action.h"
#include "Script/SkyUI.h"

class Control
{
public:
	using VM = RE::BSScript::IVirtualMachine;

	enum class Behavior : std::int32_t
	{
		Normal,
		Disable,
		Hide,
		Skip,
	};

	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t = 0;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID);
	virtual void InvokeAction(VM* a_vm);
	virtual void ResetToDefault();

	auto GetFlags() -> SkyUI::Flags;
	auto GetDesiredBehavior() -> Behavior;
	void RefreshFlags(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID);

	std::int32_t Position = -1;
	std::string ID;
	std::string Text;
	std::string Help;
	std::shared_ptr<GroupConditionTree> GroupCondition;
	Behavior GroupBehavior = Behavior::Disable;
	std::shared_ptr<Action> Action;
};

class EmptyControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
};

class HeaderControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
};

class TextControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetValue() -> std::string;

	std::string Value;
	std::string DefaultValue;

	std::string ModName;
	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
};

class ToggleControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetValue() -> bool;

	std::uint32_t GroupControl = 0;
	std::shared_ptr<ValueSource> ValueSource;
};

class SliderControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetValue() -> float;

	float Min = 0.0f;
	float Max = 0.0f;
	float Step = 0.0f;
	std::string FormatString = "{0}";
	std::shared_ptr<ValueSource> ValueSource;
};

class StepperControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetValue() -> std::int32_t;
	auto GetText() -> std::string;

	std::vector<std::string> Options;
	std::shared_ptr<ValueSource> ValueSource;
};

class MenuControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetValue() -> std::string;
	auto GetDefaultValue() -> std::string;
	auto GetShortText() -> std::string;
	auto GetIndex(const std::string&) -> std::int32_t;

	std::vector<std::string> Options;
	std::vector<std::string> ShortNames;
	std::string DefaultValue;

	std::string ModName;
	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
};

class EnumControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetValue() -> std::int32_t;
	auto GetShortText() -> std::string;

	std::vector<std::string> Options;
	std::vector<std::string> ShortNames;
	std::shared_ptr<ValueSource> ValueSource;
};

class ColorControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetColor() -> std::uint32_t;

	std::shared_ptr<ValueSource> ValueSource;
};

class KeyMapControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetKeyCode() -> std::int32_t;

	bool IgnoreConflicts = false;
	std::shared_ptr<ValueSource> ValueSource;
};

class InputControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;
	virtual void ResetToDefault() override;

	auto GetValue() -> std::string;

	std::string DefaultValue;

	std::string ModName;
	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
};

class HiddenToggleControl : public ToggleControl
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
};
