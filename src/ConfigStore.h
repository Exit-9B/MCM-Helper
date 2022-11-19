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
	void findMergedModName(std::string& modName);
	void CheckMerges();
	bool ReadConfig(const std::string& a_modName, const ScriptObjectPtr a_configScript);
	[[nodiscard]] std::shared_ptr<Config> GetConfig(const std::string& a_modName) const;
	[[nodiscard]] std::shared_ptr<Config> GetConfig(const RE::TESQuest* a_configQuest) const;

	void ClearConfigs();

private:
	ConfigStore();

	bool MakeErrorPage(
		const std::string& a_modName,
		const ScriptObjectPtr& a_configScript,
		std::shared_ptr<Config> a_config,
		const std::string& a_error);

	[[nodiscard]] RE::TESQuest* GetFormFromScript(const ScriptObjectPtr& a_configScript);
	[[nodiscard]] std::string GetModName(const ScriptObjectPtr& a_configScript);

	Storage _configStore;
	std::filesystem::path configPath{ "Data/MCM/Config"sv };
	std::map<std::string, std::string> reverseMergeMap{};
	const std::vector<std::string> extensions{ ".esp", ".esm", ".esl" };
};
