#pragma once

class TextSource
{
public:
	virtual auto GetValue() -> std::string = 0;
	virtual void SetValue(std::string_view a_value) = 0;
	virtual auto GetDefaultValue() -> std::string = 0;
	virtual void ResetToDefault() = 0;
};

class PropertyString : public TextSource
{
public:
	auto GetValue() -> std::string override;
	void SetValue(std::string_view a_value) override;
	auto GetDefaultValue() -> std::string override;
	void ResetToDefault() override;

	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
	std::string DefaultValue;
};

class ModSettingString : public TextSource
{
public:
	auto GetValue() -> std::string override;
	void SetValue(std::string_view a_value) override;
	auto GetDefaultValue() -> std::string override;
	void ResetToDefault() override;

	std::string ID;
	std::string ModName;
};