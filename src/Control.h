#pragma once

#include "ValueSource.h"
#include "Action.h"
#include "SkyUI.h"

class Control
{
public:
	using VM = RE::BSScript::IVirtualMachine;

	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t = 0;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID);
	virtual void InvokeAction(VM* a_vm);

	auto GetFlags() -> SkyUI::Flags;
	void RefreshFlags(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID);

	std::int32_t Position = -1;
	std::string ID;
	std::string Help;
	std::uint32_t GroupCondition = 0;
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

	std::string Text;
};

class TextControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;

	auto GetValue() -> std::string;

	std::string Text;
	std::string Value;

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

	auto GetValue() -> bool;

	std::string Text;
	std::uint32_t GroupControl = 0;
	SkyUI::Flags GroupMode = SkyUI::Flags::Disable;
	std::shared_ptr<ValueSource> ValueSource;
};

class SliderControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;

	auto GetValue() -> float;

	std::string Text;
	float Min = 0.0f;
	float Max = 0.0f;
	float Step = 0.0f;
	std::string FormatString = "{0}";
	std::shared_ptr<ValueSource> ValueSource;
};

class MenuControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;

	auto GetValue() -> std::string;

	std::string Text;
	std::vector<std::string> Options;

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

	auto GetValue() -> std::int32_t;
	auto GetText() -> std::string;

	std::string Text;
	std::vector<std::string> Options;
	std::shared_ptr<ValueSource> ValueSource;
};

class ColorControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;

	auto GetColor() -> std::uint32_t;

	std::string Text;
	std::shared_ptr<ValueSource> ValueSource;
};

class KeyMapControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;

	auto GetKeyCode() -> std::int32_t;

	std::string Text;
	bool IgnoreConflicts = false;
	std::shared_ptr<ValueSource> ValueSource;
};

class InputControl : public Control
{
public:
	[[nodiscard]] virtual auto Add(const ScriptObjectPtr& a_configScript) -> std::int32_t override;
	virtual void Refresh(const ScriptObjectPtr& a_configScript, std::int32_t a_optionID) override;
	virtual void InvokeAction(VM* a_vm) override;

	auto GetValue() -> std::string;

	std::string Text;

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
