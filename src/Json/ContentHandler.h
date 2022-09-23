#pragma once

#include "Json/IHandler.h"
#include "Json/ValueOptionsHandler.h"
#include "Config/PageContent.h"
#include "Config/Control.h"

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

	bool IgnoreConflicts = false;

	ValueOptionsData ValueOptions;
};

class ContentHandler final : public IHandler
{
public:
	ContentHandler(ReaderHandler* master, PageLayout* pageLayout, const ScriptObjectPtr& script);

	bool Bool(bool b) override;
	bool Uint(unsigned i) override;
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
		Control,
		ID,
		Position,
		Text,
		Help,
		Type,
		GroupBehavior,
		GroupControl,
		FormatString, // Kept for backwards compatibility with v1.0.3 - 4
		IgnoreConflicts,
	};

	State _state = State::Init;

	PageLayout* _pageLayout;
	std::string _modName;
	RE::TESForm* _form;
	std::string _scriptName;

	ControlData _data;
};
