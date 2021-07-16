#pragma once

#include <rapidjson/reader.h>
#include "Json/GroupConditionHandler.h"
#include "Json/ValueOptionsHandler.h"
#include "Json/ActionHandler.h"
#include "PageContent.h"
#include "Control.h"

struct ControlData
{
	std::string Type;
	std::int32_t Position = -1;
	std::string Text;
	std::string ID;
	std::string Help;
	std::shared_ptr<GroupConditionTree> GroupCondition;
	Control::Behavior GroupBehavior = Control::Behavior::Disable;
	std::shared_ptr<Action> Action;

	std::uint32_t GroupControl = 0;
	std::string FormatString = "{0}";

	bool IgnoreConflicts = false;

	ValueOptionsData ValueOptions;
};

class ContentHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	ContentHandler(PageLayout* pageLayout, const ScriptObjectPtr& script);

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
		Control,
		ID,
		Position,
		Text,
		Help,
		Type,
		GroupBehavior,
		GroupControl,
		FormatString,
		IgnoreConflicts,
		GroupCondition,
		Action,
		ValueOptions,
	};

	State _state = State::End;

	PageLayout* _pageLayout;
	std::string _modName;
	RE::TESForm* _form;
	std::string _scriptName;

	ControlData _data;
	std::unique_ptr<GroupConditionHandler> _groupCondition;
	std::unique_ptr<ValueOptionsHandler> _valueOptions;
	std::unique_ptr<ActionHandler> _action;
};
