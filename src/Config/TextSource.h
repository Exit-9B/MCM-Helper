#pragma once

class TextSource
{
public:
	[[nodiscard]] virtual std::string GetValue() const = 0;
	virtual void SetValue(std::string_view a_value) = 0;
	[[nodiscard]] virtual std::string GetDefaultValue() const = 0;
	virtual void ResetToDefault() = 0;
};

class PropertyString : public TextSource
{
public:
	[[nodiscard]] std::string GetValue() const override;
	void SetValue(std::string_view a_value) override;
	[[nodiscard]] std::string GetDefaultValue() const override;
	void ResetToDefault() override;

	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
	std::string DefaultValue;
};

class ModSettingString : public TextSource
{
public:
	[[nodiscard]] std::string GetValue() const override;
	void SetValue(std::string_view a_value) override;
	[[nodiscard]] std::string GetDefaultValue() const override;
	void ResetToDefault() override;

	std::string ID;
	std::string ModName;
};
