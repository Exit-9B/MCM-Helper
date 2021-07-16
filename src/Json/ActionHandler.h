#pragma once

#include <rapidjson/reader.h>
#include "Json/ParamsHandler.h"
#include "Action.h"

struct ActionData
{
	std::string Type;
	RE::TESForm* Form;
	std::string Script;
	std::string ScriptName;
	std::string Function;
	std::vector<std::string> Params;
};

class ActionHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	ActionHandler(
		std::shared_ptr<Action>* action,
		RE::TESForm* sourceForm,
		const std::string& scriptName);

	bool Complete();

	bool Bool(bool b);
	bool Int(int i);
	bool Uint(unsigned i);
	bool Double(double d);
	bool String(const Ch* str, SizeType length, bool copy);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);
	bool StartArray();
	bool EndArray(SizeType elementCount);

private:
	enum class State
	{
		End,
		Start,
		Type,
		Form,
		Script,
		ScriptName,
		Function,
		Params,
	};

	State _state = State::End;
	RE::TESForm* _sourceForm;
	std::string _scriptName;

	std::shared_ptr<Action>* _action;
	ActionData _data;
	std::unique_ptr<ParamsHandler> _params;
};
