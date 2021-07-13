#pragma once

#include "Config.h"
#include "GroupConditionTree.h"
#include "Action.h"
#include <nlohmann/json.hpp>

class ConfigStore
{
	using json = nlohmann::json;
	using Storage = std::unordered_map<std::string, std::shared_ptr<Config>>;

public:
	~ConfigStore() = default;
	ConfigStore(const ConfigStore&) = delete;
	ConfigStore(ConfigStore&&) = delete;
	ConfigStore& operator=(const ConfigStore&) = delete;
	ConfigStore& operator=(ConfigStore&&) = delete;

	static ConfigStore& GetInstance();

	void ReadConfigs();

	bool ReadConfig(ScriptObjectPtr a_configScript);

	auto GetConfig(const std::string& a_modName) -> std::shared_ptr<Config>;

	auto GetConfig(RE::TESQuest* a_configQuest) -> std::shared_ptr<Config>;

private:
	ConfigStore();

	auto ReadPage(
		ScriptObjectPtr& a_configScript,
		const json& a_page)
		-> std::shared_ptr<PageContent>;

	auto ReadCustomContent(const json& a_customContent)
		-> std::shared_ptr<CustomContent>;

	auto ReadContent(
		ScriptObjectPtr& a_configScript,
		const json& a_content)
		-> std::shared_ptr<PageLayout>;

	auto ReadValueSource(
		ScriptObjectPtr& a_configScript,
		const std::string& a_ID,
		const json& a_valueOptions)
		-> std::shared_ptr<ValueSource>;

	auto ReadGroupCondition(const json& a_groupCondition) -> std::shared_ptr<GroupConditionTree>;

	auto ReadAction(
		ScriptObjectPtr& a_configScript,
		const json& a_action) -> std::shared_ptr<Action>;

	auto GetFormFromScript(ScriptObjectPtr& a_configScript) -> RE::TESQuest*;

	auto GetModName(ScriptObjectPtr& a_configScript) -> std::string;

	Storage _configStore;
};