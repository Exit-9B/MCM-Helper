#pragma once

struct GroupConditionTree
{
	enum class ConjunctionType : std::int8_t
	{
		OR,
		AND,
		ONLY,
		NOT,
	};

	ConjunctionType Conjunction = ConjunctionType::OR;
	std::vector<std::uint32_t> TopLevelOperands;
	std::vector<std::shared_ptr<GroupConditionTree>> SubTrees;

	auto GetIsActive() -> bool const;
};