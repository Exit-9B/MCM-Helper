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

class ActionHandler final : public IHandler
{
public:
	ActionHandler(
		ReaderHandler* master,
		std::shared_ptr<Action>* action,
		RE::TESForm* sourceForm = nullptr,
		const std::string& scriptName = ""s);

	bool String(const Ch* str, SizeType length, bool copy) override;
	bool StartObject() override;
	bool Key(const Ch* str, SizeType length, bool copy) override;
	bool EndObject(SizeType memberCount) override;

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

	std::shared_ptr<Action>* _action;
	ActionData _data;
};
