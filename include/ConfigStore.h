#pragma once

#include "Config/Config.h"
#include "Config/GroupConditionTree.h"
#include "Config/Action.h"

class ConfigStore
{
	using Storage = std::unordered_map<std::string, std::shared_ptr<Config>>;

public:
	~ConfigStore() = default;
	ConfigStore(const ConfigStore&) = delete;
	ConfigStore(ConfigStore&&) = delete;
	ConfigStore& operator=(const ConfigStore&) = delete;
	ConfigStore& operator=(ConfigStore&&) = delete;

	static auto GetInstance() -> ConfigStore&;

	void ReadConfigs();
	bool ReadConfig(const std::string& a_modName, ScriptObjectPtr a_configScript);
	auto GetConfig(const std::string& a_modName) -> std::shared_ptr<Config>;
	auto GetConfig(RE::TESQuest* a_configQuest) -> std::shared_ptr<Config>;

private:
	ConfigStore();

	auto GetFormFromScript(ScriptObjectPtr& a_configScript) -> RE::TESQuest*;
	auto GetModName(ScriptObjectPtr& a_configScript) -> std::string;

	Storage _configStore;
};