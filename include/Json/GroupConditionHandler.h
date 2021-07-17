#pragma once

#include "Json/ReaderHandler.h"
#include "Config/GroupConditionTree.h"

class GroupConditionHandler : public IHandler
{
public:
	GroupConditionHandler(ReaderHandler* master, std::shared_ptr<GroupConditionTree> tree);

	bool Uint(unsigned i);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);
	bool StartArray();
	bool EndArray(SizeType elementCount);

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

	ReaderHandler* _master;
	std::shared_ptr<GroupConditionTree> _tree;
};