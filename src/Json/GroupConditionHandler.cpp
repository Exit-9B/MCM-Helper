#include "Json/GroupConditionHandler.h"

GroupConditionHandler::GroupConditionHandler(
	ReaderHandler* master,
	std::shared_ptr<GroupConditionTree> tree) :
	_master{ master },
	_tree{ tree }
{
}

bool GroupConditionHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::End:
		_tree->TopLevelOperands.push_back(static_cast<std::uint32_t>(i));
		_master->PopHandler();
		return true;
	case State::Conjunction:
		_tree->TopLevelOperands.push_back(static_cast<std::uint32_t>(i));
		_state = State::Start;
		return true;
	case State::Array:
	case State::TopLevelArray:
		_tree->TopLevelOperands.push_back(static_cast<std::uint32_t>(i));
		return true;
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
	case State::TopLevelArray:
	{
		auto tree = std::make_shared<GroupConditionTree>();
		_tree->SubTrees.push_back(tree);
		_master->PushHandler<GroupConditionHandler>(_master, tree);
		return true;
	}
	default:
		return false;
	}
}

bool GroupConditionHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
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
	default:
		return false;
	}
}

bool GroupConditionHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Start:
		_master->PopHandler();
		return true;
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
	default:
		return false;
	}
}

bool GroupConditionHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	switch (_state) {
	case State::Array:
		_state = State::Start;
		return true;
	case State::TopLevelArray:
		_master->PopHandler();
		return true;
	default:
		return false;
	}
}