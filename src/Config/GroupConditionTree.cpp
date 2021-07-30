#include "Config/GroupConditionTree.h"
#include "ConfigPageCache.h"

auto GroupConditionTree::GetIsActive() -> bool const
{
	auto& configPageCache = ConfigPageCache::GetInstance();

	switch (Conjunction) {
	case ConjunctionType::OR:
	{
		for (auto operand : TopLevelOperands) {
			if (configPageCache.IsGroupActive(operand)) {
				return true;
			}
		}

		for (auto& subtree : SubTrees) {
			if (!subtree || subtree->GetIsActive()) {
				return true;
			}
		}

		return false;
	}

	case ConjunctionType::AND:
	{
		for (auto operand : TopLevelOperands) {
			if (!configPageCache.IsGroupActive(operand)) {
				return false;
			}
		}

		for (auto& subtree : SubTrees) {
			if (!subtree)
				continue;

			if (!subtree->GetIsActive()) {
				return false;
			}
		}

		return true;
	}

	case ConjunctionType::ONLY:
	{
		if (!SubTrees.empty()) {
			return false;
		}

		auto groups = configPageCache.GetGroups();

		for (auto operand : TopLevelOperands) {
			if (!configPageCache.IsGroupActive(operand)) {
				return false;
			}

			groups.erase(operand);
		}

		for (auto group : groups) {
			if (configPageCache.IsGroupActive(group)) {
				return false;
			}
		}

		return true;
	}

	case ConjunctionType::NOT:
	{
		for (auto operand : TopLevelOperands) {
			if (configPageCache.IsGroupActive(operand)) {
				return false;
			}
		}

		for (auto& subtree : SubTrees) {
			if (!subtree)
				continue;

			if (subtree->GetIsActive()) {
				return false;
			}
		}

		return true;
	}

	default:
	{
		return true;
	}
	}
}