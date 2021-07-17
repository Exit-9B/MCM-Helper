#pragma once

#include "Json/ReaderHandler.h"
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

class ActionHandler : public IHandler
{
public:
	ActionHandler(
		ReaderHandler* master,
		std::shared_ptr<Action>* action,
		RE::TESForm* sourceForm,
		const std::string& scriptName);

	bool String(const Ch* str, SizeType length, bool copy);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);

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
	};

	State _state = State::End;
	ReaderHandler* _master;
	RE::TESForm* _sourceForm;
	std::string _scriptName;

	std::shared_ptr<Action>* _action;
	ActionData _data;
};
