#pragma once

#include "Config/Config.h"
#include "Config/GroupConditionTree.h"
#include "Config/Action.h"

class ConfigStore final
{
	using Storage = std::unordered_map<std::string, std::shared_ptr<Config>>;

public:
	~ConfigStore() = default;
	ConfigStore(const ConfigStore&) = delete;
	ConfigStore(ConfigStore&&) = delete;
	ConfigStore& operator=(const ConfigStore&) = delete;
	ConfigStore& operator=(ConfigStore&&) = delete;

	[[nodiscard]] static ConfigStore& GetInstance();

	void ReadConfigs();
	bool ReadConfig(const std::string& a_modName, ScriptObjectPtr a_configScript);
	[[nodiscard]] auto GetConfig(const std::string& a_modName) -> std::shared_ptr<Config>;
	[[nodiscard]] auto GetConfig(RE::TESQuest* a_configQuest) -> std::shared_ptr<Config>;

	void ClearConfigs();

private:
	ConfigStore();

	bool MakeErrorPage(
		const std::string& a_modName,
		ScriptObjectPtr& a_configScript,
		std::shared_ptr<Config> a_config,
		const std::string& a_error);

	[[nodiscard]] RE::TESQuest* GetFormFromScript(ScriptObjectPtr& a_configScript);
	[[nodiscard]] std::string GetModName(ScriptObjectPtr& a_configScript);

	Storage _configStore;
};
