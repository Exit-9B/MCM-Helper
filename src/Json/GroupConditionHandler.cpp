#include "Json/GroupConditionHandler.h"

GroupConditionHandler::GroupConditionHandler(
	ReaderHandler* master,
	std::shared_ptr<GroupConditionTree> tree) :
	IHandler{ master },
	_tree{ tree }
{
}

bool GroupConditionHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Init:
		_tree->TopLevelOperands.push_back(static_cast<std::uint32_t>(i));
		_master->PopHandler();
		return true;
	case State::Conjunction:
		_tree->TopLevelOperands.push_back(static_cast<std::uint32_t>(i));
		_state = State::Main;
		return true;
	case State::Array:
	case State::TopLevelArray:
		_tree->TopLevelOperands.push_back(static_cast<std::uint32_t>(i));
		return true;
	default:
		return IHandler::Uint(i);
	}
}

bool GroupConditionHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	case State::Array:
	case State::TopLevelArray:
	{
		auto tree = std::make_shared<GroupConditionTree>();
		_tree->SubTrees.push_back(tree);
		_master->PushHandler<GroupConditionHandler>(tree);
		return true;
	}
	default:
		return IHandler::StartObject();
	}
}

bool GroupConditionHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Init:
	case State::Main:
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
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool GroupConditionHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndObject(memberCount);
	}
}

bool GroupConditionHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::TopLevelArray;
		return true;
	case State::Conjunction:
		_state = State::Array;
		return true;
	default:
		return IHandler::StartArray();
	}
}

bool GroupConditionHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Array:
		_state = State::Main;
		return true;
	case State::TopLevelArray:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}
