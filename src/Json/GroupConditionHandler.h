#pragma once

#include <rapidjson/reader.h>
#include "GroupConditionTree.h"

class GroupConditionHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	GroupConditionHandler(std::shared_ptr<GroupConditionTree> tree);

	bool Complete();

	bool Uint(unsigned i);
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
		Conjunction,
		Array,
		Child,
		TopLevelArray,
		TopLevelArrayChild,
	};

	State _state = State::End;
	std::shared_ptr<GroupConditionTree> _tree;
	std::unique_ptr<GroupConditionHandler> _child;
};