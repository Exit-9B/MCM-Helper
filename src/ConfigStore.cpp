#include "ConfigStore.h"
#include "KeybindManager.h"
#include "FormUtil.h"
#include "Script/SkyUI.h"
#include "Script/ScriptObject.h"
#include "Json/ReaderHandler.h"
#include "Json/ConfigHandler.h"

#undef GetObject

ConfigStore::ConfigStore()
{
	logger::info("ModConfigStore initializing."sv);
}

ConfigStore& ConfigStore::GetInstance()
{
	static ConfigStore instance{};
	return instance;
}

void ConfigStore::ReadConfigs()
{
	auto startTime = std::chrono::steady_clock::now();

	auto configManager = SkyUI::ConfigManager::GetInstance();
	if (!configManager) {
		// Didn't find SkyUI
		logger::warn("Could not find SkyUI Config Manager."sv);
		return;
	}

	auto modConfigsVar = ScriptObject::GetVariable(configManager, "_modConfigs"sv);
	auto modConfigs = modConfigsVar && modConfigsVar->IsObjectArray()
		? modConfigsVar->GetArray()
		: nullptr;

	if (modConfigs) {
		for (auto& modConfig : *modConfigs) {
			auto configScript = modConfig.GetObject();
			if (configScript && ScriptObject::IsType(configScript, "MCM_ConfigBase")) {
				auto modName = GetModName(configScript);
				if (!modName.empty() && ReadConfig(modName, configScript)) {
					SkyUI::ConfigManager::UpdateDisplayName(configManager, configScript);
					KeybindManager::GetInstance().ReadKeybinds(modName);
				}
			}
		}
	}

	// Handle 'SkyUI - Resort' by Barzing
	for (std::int32_t i = 0; i <= 9; i++) {
		auto modConfigsVarName =
			i == 0 ? "_MainMenu"s
			: fmt::format("_modConfigsP{}"sv, i);

		auto modConfigsPVar = ScriptObject::GetVariable(configManager, modConfigsVarName);
		auto modConfigsP = modConfigsPVar && modConfigsPVar->IsArray() ?
            modConfigsPVar->GetArray() :
            nullptr;

		if (!modConfigsP) {
			continue;
		}

		for (auto& modConfig : *modConfigsP) {
			auto configScript = modConfig.GetObject();
			if (configScript && ScriptObject::IsType(configScript, "MCM_ConfigBase")) {
				auto modName = GetModName(configScript);
				if (!modName.empty() && ReadConfig(modName, configScript)) {
					SkyUI::ConfigManager::UpdateDisplayName_Barzing(
						configManager,
						configScript,
						i);

					KeybindManager::GetInstance().ReadKeybinds(modName);
				}
			}
		}
	}

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	logger::info("Registered {} mod configs in {} ms."sv, _configStore.size(), elapsedMs.count());
}
void ConfigStore::findMergedModName(std::string& modName)
{
	if (g_mergeMapperInterface && !reverseMergeMap.empty()) {
		for (const auto& extension : extensions) {
			auto modConfigFullName = modName + extension;
			std::transform(
				modConfigFullName.begin(),
				modConfigFullName.end(),
				modConfigFullName.begin(),
				[](auto ch)
				{
					return static_cast<char>(std::tolower(ch));
				});
			if (auto search = reverseMergeMap.find(modConfigFullName);
				search != reverseMergeMap.end()) {
				const auto& mergedModName = search->second;
				logger::debug("\t\tUsing default merged {} for {} using reverseMergeMap", mergedModName, modName);
				modName = mergedModName.substr(0, mergedModName.find(extension));
			}
		}
	}
}
void ConfigStore::CheckMerges()
{
	logger::debug("Searching for MCM Config files within {}", configPath.string());

	for (const auto& entry : std::filesystem::directory_iterator(configPath)) {
		if (entry.exists() && entry.is_directory()) {
			auto modPath{ entry.path().generic_string() };
			logger::debug("Found potential mod {}", modPath);
			auto file = modPath + "\\config.json";
			if (std::filesystem::exists(file)) {
				auto modConfigName = modPath.substr(configPath.generic_string().size() + 1);
				logger::debug("\tConfirmed config {} for {}", file, modConfigName);
				if (g_mergeMapperInterface) {
					for (const auto& extension : extensions) {
						auto modConfigFullName = modConfigName + extension;
						const auto mergePair = g_mergeMapperInterface->GetNewFormID(
							modConfigFullName.c_str(),
							0);
						if (std::strcmp(mergePair.first, modConfigFullName.c_str())) {
							logger::debug(
								"\tFound default merged {} for {}",
								mergePair.first,
								modConfigFullName);
							reverseMergeMap.emplace(mergePair.first, modConfigFullName);
						}
					}
				}
			}
			else {
				logger::warn("\tMCM missing config file {}", file);
			}
		}
	}
	if (!reverseMergeMap.empty())
		logger::info("ReverseMergeMap has {} entries.", reverseMergeMap.size());
}

bool ConfigStore::ReadConfig(const std::string& a_modName, ScriptObjectPtr a_configScript)
{
	assert(a_configScript);

	auto configLocation = configPath / a_modName / "config.json"sv;

	ReaderHandler handler;
	auto config = std::make_shared<Config>();
	handler.PushHandler<ConfigHandler>(config.get(), a_modName, a_configScript);

	if (!handler.ReadFile(configLocation)) {
		logger::warn("Failed to parse config for {}"sv, a_modName);
		if (MakeErrorPage(a_modName, a_configScript, config, handler.GetError())) {
			_configStore[a_modName] = config;
			return true;
		}
	}

	_configStore[a_modName] = config;

	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;
	auto pagesVariable = a_configScript->GetProperty("Pages");

	if (vm && pagesVariable) {
		ScriptArrayPtr pagesArray;
		vm->CreateArray(
			RE::BSScript::TypeInfo{ RE::BSScript::TypeInfo::RawType::kString },
			static_cast<std::uint32_t>(config->SubPages.size()),
			pagesArray);

		std::uint32_t index = 0;
		for (auto& [pageDisplayName, pageContent] : config->SubPages) {
			pagesArray->data()[index].SetString(pageDisplayName);
			index++;
		}

		pagesVariable->SetArray(pagesArray);
	}

	return true;
}

std::shared_ptr<Config> ConfigStore::GetConfig(const std::string& a_modName) const
{
	auto it = _configStore.find(a_modName);
	return it != _configStore.end() ? it->second : nullptr;
}

std::shared_ptr<Config> ConfigStore::GetConfig(const RE::TESQuest* a_configQuest) const
{
	auto modName = FormUtil::GetModName(a_configQuest);
	return GetConfig(modName);
}

bool ConfigStore::MakeErrorPage(
	const std::string& a_modName,
	const ScriptObjectPtr& a_configScript,
	std::shared_ptr<Config> a_config,
	const std::string& a_error)
{
	assert(a_config);

	auto displayName = a_configScript->GetProperty("ModName"sv);
	if (!displayName) {
		return false;
	}

	if (displayName->GetString().empty()) {
		displayName->SetString(a_modName);
	}

	auto page = std::make_shared<PageLayout>();

	auto control = std::make_shared<ErrorControl>();
	control->Error = !a_error.empty() ? a_error : "Unknown error; check JSON syntax"s;

	page->Controls.push_back(control);
	a_config->MainPage = page;

	return true;
}

void ConfigStore::ClearConfigs()
{
	_configStore.clear();
}

RE::TESQuest* ConfigStore::GetFormFromScript(const ScriptObjectPtr& a_configScript)
{
	auto typeID = static_cast<RE::VMTypeID>(RE::TESQuest::FORMTYPE);
	auto quest = static_cast<RE::TESQuest*>(a_configScript->Resolve(typeID));
	return quest;
}

std::string ConfigStore::GetModName(const ScriptObjectPtr& a_configScript)
{
	auto quest = GetFormFromScript(a_configScript);
	return FormUtil::GetModName(quest);
}
