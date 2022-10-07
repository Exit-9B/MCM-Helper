#pragma once

#include "Config/ValueSource.h"
#include "Config/TextSource.h"
#include "Config/GroupConditionTree.h"
#include "Config/Action.h"
#include "Script/SkyUI.h"

class Control
{
public:
	enum class Behavior : std::int32_t
	{
		Normal,
		Disable,
		Hide,
		Skip,
	};

	[[nodiscard]] virtual std::int32_t Add(const ScriptObjectPtr& a_configScript) = 0;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID);
	virtual VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm);
	virtual void ResetToDefault();
	[[nodiscard]] virtual std::string GetInfoText() const;
	[[nodiscard]] virtual std::string GetValueString() const;
	[[nodiscard]] virtual SkyUI::Flags GetFlags() const;

	[[nodiscard]] auto GetDesiredBehavior() const -> Behavior;
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
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
};

class HeaderControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
};

class TextControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;
	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] std::string GetValue() const;

	std::string Value;
	std::shared_ptr<TextSource> ValueSource;
};

class ToggleControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;
	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] bool GetValue() const;

	std::uint32_t GroupControl = 0;
	std::shared_ptr<ValueSource> ValueSource;
};

class SliderControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;

	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] float GetValue() const;

	float Min = 0.0f;
	float Max = 0.0f;
	float Step = 0.0f;
	std::string FormatString = "{0}";
	std::shared_ptr<ValueSource> ValueSource;
};

class StepperControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;

	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] std::int32_t GetValue() const;
	[[nodiscard]] std::string GetText() const;

	std::vector<std::string> Options;
	std::shared_ptr<ValueSource> ValueSource;
};

class MenuControl : public MenuDialogControl
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;

	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] std::string GetValue() const;
	[[nodiscard]] std::string GetDefaultValue() const;
	[[nodiscard]] std::string GetShortText() const;

	std::shared_ptr<TextSource> ValueSource;
};

class EnumControl : public MenuDialogControl
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;

	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] std::int32_t GetValue() const;
	[[nodiscard]] std::string GetShortText() const;

	std::shared_ptr<ValueSource> ValueSource;
};

class ColorControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;
	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] std::uint32_t GetColor() const;

	std::shared_ptr<ValueSource> ValueSource;
};

class KeyMapControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;
	[[nodiscard]] std::string GetInfoText() const override;
	[[nodiscard]] std::string GetValueString() const override;
	[[nodiscard]] SkyUI::Flags GetFlags() const override;

	[[nodiscard]] std::uint32_t GetKeyCode() const;
	[[nodiscard]] std::string GetDescription() const;

	std::string ModName;
	bool IgnoreConflicts = false;
	std::shared_ptr<ValueSource> ValueSource;
};

class InputControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	VMAwaitable InvokeAction(RE::BSScript::IVirtualMachine* a_vm) override;
	void ResetToDefault() override;

	[[nodiscard]] std::string GetValueString() const override;

	[[nodiscard]] std::string GetValue() const;

	std::shared_ptr<TextSource> ValueSource;
};

class HiddenToggleControl : public ToggleControl
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;
	void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
};

class ErrorControl : public Control
{
public:
	[[nodiscard]] std::int32_t Add(const ScriptObjectPtr& a_configScript) override;

	std::string Error;
};
