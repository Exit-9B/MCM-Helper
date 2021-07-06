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

	auto ReadPage(const std::string& a_modName, const json& a_page)
		-> std::shared_ptr<PageContent>;

	auto ReadCustomContent(const json& a_customContent)
		-> std::shared_ptr<CustomContent>;

	auto ReadContent(const std::string& a_modName, const json& a_content)
		-> std::shared_ptr<PageLayout>;

	auto ReadValueSource(
		const std::string& a_modName,
		const std::string& a_ID,
		const json& a_valueOptions)
		-> std::shared_ptr<ValueSource>;

	auto ReadGroupCondition(const json& a_groupCondition) -> std::shared_ptr<GroupConditionTree>;

	auto ReadAction(const json& a_action) -> std::shared_ptr<Action>;

	Storage _configStore;
};