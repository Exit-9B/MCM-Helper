#include "Json/GroupConditionHandler.h"

GroupConditionHandler::GroupConditionHandler(std::shared_ptr<GroupConditionTree> tree) :
	_tree{ tree }
{
}

bool GroupConditionHandler::Complete()
{
	return _state == State::End;
}

bool GroupConditionHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::End:
		_tree->TopLevelOperands.push_back(i);
		return true;
	case State::Conjunction:
		_tree->TopLevelOperands.push_back(i);
		_state = State::Start;
		return true;
	case State::Array:
	case State::TopLevelArray:
		_tree->TopLevelOperands.push_back(i);
		return true;
	case State::Child:
	case State::TopLevelArrayChild:
		return _child->Uint(i);
	default:
		return false;
	}
}

bool GroupConditionHandler::StartObject()
{
	switch (_state) {
	case State::End:
		_state = State::Start;
		return true;
	case State::Array:
	{
		auto tree = std::make_shared<GroupConditionTree>();
		_tree->SubTrees.push_back(tree);
		_child = std::make_unique<GroupConditionHandler>(tree);
		_state = State::Child;
		return true;
	}
	case State::TopLevelArray:
	{
		auto tree = std::make_shared<GroupConditionTree>();
		_tree->SubTrees.push_back(tree);
		_child = std::make_unique<GroupConditionHandler>(tree);
		_state = State::TopLevelArrayChild;
		return true;
	}
	case State::Child:
	case State::TopLevelArrayChild:
		return _child->StartObject();
	default:
		return false;
	}
}

bool GroupConditionHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Start:
		if (strcmp(str, "OR") == 0) {
			_tree->Conjunction = GroupConditionTree::ConjunctionType::OR;
			_state = State::Conjunction;
			return true;
		}
		else if (strcmp(str, "AND") == 0) {
			_tree->Conjunction = GroupConditionTree::ConjunctionType::AND;
			_state = State::Conjunction;
			return true;
		}
		else if (strcmp(str, "ONLY") == 0) {
			_tree->Conjunction = GroupConditionTree::ConjunctionType::ONLY;
			_state = State::Conjunction;
			return true;
		}
		else if (strcmp(str, "NOT") == 0) {
			_tree->Conjunction = GroupConditionTree::ConjunctionType::NOT;
			_state = State::Conjunction;
			return true;
		}
		else {
			return false;
		}
	case State::Child:
	case State::TopLevelArrayChild:
		return _child->Key(str, length, copy);
	default:
		return false;
	}
}

bool GroupConditionHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Start:
		_state = State::End;
		return true;
	case State::Child:
	{
		bool childOK = _child->EndObject(memberCount);
		if (_child->Complete())
			_state = State::Array;
		return childOK;
	}
	case State::TopLevelArrayChild:
	{
		bool childOK = _child->EndObject(memberCount);
		if (_child->Complete())
			_state = State::TopLevelArray;
		return childOK;
	}
	default:
		return false;
	}
}

bool GroupConditionHandler::StartArray()
{
	switch (_state) {
	case State::End:
		_state = State::TopLevelArray;
		return true;
	case State::Conjunction:
		_state = State::Array;
		return true;
	case State::Child:
	case State::TopLevelArrayChild:
		return _child->StartArray();
	default:
		return false;
	}
}

bool GroupConditionHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Array:
		_state = State::Start;
		return true;
	case State::TopLevelArray:
		_state = State::End;
		return true;
	case State::Child:
	case State::TopLevelArrayChild:
		return _child->EndArray(elementCount);
	default:
		return false;
	}
}