#pragma once

#include "Json/IHandler.h"
#include "Config/ValueSource.h"

struct ValueOptionsData
{
	float Min = 0.0f;
	float Max = 0.0f;
	float Step = 0.0f;

	std::string Value;
	std::vector<std::string> Options;
	std::vector<std::string> ShortNames;

	std::string SourceType;
	RE::TESForm* SourceForm;
	std::string ScriptName;
	std::string PropertyName;
	float DefaultValue = 0.0f;
	std::string DefaultValueStr;
	std::shared_ptr<ValueSource> ValueSource;
};

class ValueOptionsHandler final : public IHandler
{
public:
	ValueOptionsHandler(
		ReaderHandler* master,
		ValueOptionsData* data,
		const std::string& modName,
		RE::TESForm* sourceForm,
		const std::string& scriptName);

	bool Bool(bool b) override;
	bool Int(int i) override;
	bool Uint(unsigned i) override;
	bool Double(double d) override;
	bool String(const Ch* str, SizeType length, bool copy) override;
	bool StartObject() override;
	bool Key(const Ch* str, SizeType length, bool copy) override;
	bool EndObject(SizeType memberCount) override;
	bool StartArray() override;
	bool EndArray(SizeType elementCount) override;

private:
	enum class State
	{
		Init,
		Main,
		Min,
		Max,
		Step,
		Value,
		Options,
		OptionElem,
		ShortNames,
		ShortNameElem,
		SourceType,
		SourceForm,
		ScriptName,
		PropertyName,
		DefaultValue,
	};

	State _state = State::Init;

	ValueOptionsData* _data;
	std::string _modName;
	RE::TESForm* _sourceForm;
	std::string _scriptName;
};
