#pragma once

#include "Json/IHandler.h"
#include "Config/GroupConditionTree.h"

class GroupConditionHandler final : public IHandler
{
public:
	GroupConditionHandler(ReaderHandler* master, std::shared_ptr<GroupConditionTree> tree);

	bool Uint(unsigned i) override;
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
		Conjunction,
		Array,
		TopLevelArray,
	};

	State _state = State::Init;

	std::shared_ptr<GroupConditionTree> _tree;
};