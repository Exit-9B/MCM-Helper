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

auto ConfigStore::GetInstance() -> ConfigStore&
{
	static ConfigStore instance;
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

bool ConfigStore::ReadConfig(const std::string& a_modName, ScriptObjectPtr a_configScript)
{
	assert(a_configScript);

	std::filesystem::path configPath{ "Data/MCM/Config"sv };
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

auto ConfigStore::GetConfig(const std::string& a_modName) -> std::shared_ptr<Config>
{
	auto it = _configStore.find(a_modName);
	if (it != _configStore.end()) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

auto ConfigStore::GetConfig(RE::TESQuest* a_configQuest) -> std::shared_ptr<Config>
{
	auto modName = FormUtil::GetModName(a_configQuest);
	return GetConfig(modName);
}

bool ConfigStore::MakeErrorPage(
	const std::string& a_modName,
	ScriptObjectPtr& a_configScript,
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

auto ConfigStore::GetFormFromScript(ScriptObjectPtr& a_configScript) -> RE::TESQuest*
{
	auto typeID = static_cast<RE::VMTypeID>(RE::TESQuest::FORMTYPE);
	auto quest = static_cast<RE::TESQuest*>(a_configScript->Resolve(typeID));
	return quest;
}

auto ConfigStore::GetModName(ScriptObjectPtr& a_configScript) -> std::string
{
	auto quest = GetFormFromScript(a_configScript);
	return FormUtil::GetModName(quest);
}
