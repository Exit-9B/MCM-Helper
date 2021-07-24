#pragma once

#include "Json/IHandler.h"
#include "Config/Action.h"

struct ActionData
{
	std::string Type;
	RE::TESForm* Form;
	std::string Script;
	std::string ScriptName;
	std::string Function;
	std::string Command;
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

	ActionHandler(
		ReaderHandler* master,
		std::shared_ptr<Action>* action,
		const std::string& id);

	bool String(const Ch* str, SizeType length, bool copy);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);

private:
	enum class State
	{
		Init,
		Main,
		Type,
		Form,
		Script,
		ScriptName,
		Function,
		Command,
	};

	State _state = State::Init;

	RE::TESForm* _sourceForm;
	std::string _scriptName;
	std::string _id;

	std::shared_ptr<Action>* _action;
	ActionData _data;
};
