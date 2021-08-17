#pragma once

#include "Config/ValueSource.h"
#include "Config/TextSource.h"
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
	virtual auto GetInfoText() -> std::string;
	virtual auto GetValueString() -> std::string;
	virtual auto GetFlags() -> SkyUI::Flags;

	auto GetDesiredBehavior() -> Behavior;
	void RefreshFlags(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID);

	std::int32_t Position = -1;
	std::string ID;
	std::string Text;
	std::string Help;
	std::shared_ptr<GroupConditionTree> GroupCondition;
	Behavior GroupBehavior = Behavior::Disable;
	std::shared_ptr<Function> Action;
};

class MenuDialogControl : public Control
{
public:
	std::vector<std::string> Options;
	std::vector<std::string> ShortNames;
};

class EmptyControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
};

class HeaderControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
};

class TextControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

	auto GetValue() -> std::string;

	std::string Value;
	std::shared_ptr<TextSource> ValueSource;
};

class ToggleControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

	auto GetValue() -> bool;

	std::uint32_t GroupControl = 0;
	std::shared_ptr<ValueSource> ValueSource;
};

class SliderControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

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
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

	auto GetValue() -> std::int32_t;
	auto GetText() -> std::string;

	std::vector<std::string> Options;
	std::shared_ptr<ValueSource> ValueSource;
};

class MenuControl : public MenuDialogControl
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

	auto GetValue() -> std::string;
	auto GetDefaultValue() -> std::string;
	auto GetShortText() -> std::string;

	std::shared_ptr<TextSource> ValueSource;
};

class EnumControl : public MenuDialogControl
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

	auto GetValue() -> std::int32_t;
	auto GetShortText() -> std::string;

	std::shared_ptr<ValueSource> ValueSource;
};

class ColorControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

	auto GetColor() -> std::uint32_t;

	std::shared_ptr<ValueSource> ValueSource;
};

class KeyMapControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetInfoText() -> std::string override;
	auto GetValueString() -> std::string override;
	auto GetFlags() -> SkyUI::Flags override;

	auto GetKeyCode() -> std::uint32_t;
	auto GetDescription() -> std::string;

	std::string ModName;
	bool IgnoreConflicts = false;
	std::shared_ptr<ValueSource> ValueSource;
};

class InputControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	void InvokeAction(VM* a_vm) override;
	void ResetToDefault() override;
	auto GetValueString() -> std::string override;

	auto GetValue() -> std::string;

	std::shared_ptr<TextSource> ValueSource;
};

class HiddenToggleControl : public ToggleControl
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
};

class ErrorControl : public Control
{
public:
	[[nodiscard]] auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;

	std::string Error;
};
