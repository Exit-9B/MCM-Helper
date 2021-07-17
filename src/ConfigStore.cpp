#include "ConfigStore.h"
#include "SkyUI.h"
#include "Utils.h"
#include "Json/ReaderHandler.h"
#include "Json/ConfigHandler.h"
#include <rapidjson/filereadstream.h>

#undef GetObject

ConfigStore::ConfigStore()
{
	logger::info("ModConfigStore initializing."sv);
}

ConfigStore& ConfigStore::GetInstance()
{
	static ConfigStore instance;
	return instance;
}

void ConfigStore::ReadConfigs()
{
	auto startTime = std::chrono::steady_clock::now();

	auto configManager = SkyUI::ConfigManager::GetInstance();
	if (!configManager)
	{
		// Didn't find SkyUI
		logger::warn("Could not find SkyUI Config Manager."sv);
		return;
	}

	auto modConfigsVar = Utils::GetVariable(configManager, "_modConfigs"sv);
	if (!modConfigsVar || !modConfigsVar->IsObjectArray())
	{
		// Something else went wrong
		logger::warn("Could not find registered mod configs."sv);
		return;
	}

	auto modConfigs = modConfigsVar->GetArray();

	if (!modConfigs)
	{
		logger::warn("SkyUI Config Manager is not ready"sv);
		return;
	}

	for (auto& modConfig : *modConfigs)
	{
		auto configScript = modConfig.GetObject();

		if (configScript)
		{
			if (ReadConfig(configScript))
			{
				SkyUI::ConfigManager::UpdateDisplayName(configManager, configScript);
			}
		}
	}

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	logger::info(
		"Registered {} mod configs in {} ms."sv,
		_configStore.size(),
		elapsedMs.count());
}

bool ConfigStore::ReadConfig(ScriptObjectPtr a_configScript)
{
	assert(a_configScript);

	auto quest = GetFormFromScript(a_configScript);
	auto plugin = Utils::GetModName(quest);

	if (plugin.empty())
		return false;

	std::filesystem::path configPath{ "Data/MCM/Config/"sv };
	auto configLocation = configPath / plugin / "config.json"sv;

	ReaderHandler handler;
	auto config = std::make_shared<Config>();
	handler.PushHandler<ConfigHandler>(&handler, config.get(), plugin, a_configScript);

	rapidjson::Reader reader;

	FILE* fp = nullptr;
	auto err = _wfopen_s(&fp, configLocation.c_str(), L"r");
	if (err != 0)
	{
		logger::warn("Failed to open config for {}"sv, plugin);
		return false;
	}

	char readBuffer[65536]{};
	rapidjson::FileReadStream is{ fp, readBuffer, sizeof(readBuffer) };

	auto result = reader.Parse(is, handler);
	fclose(fp);
	if (!result)
		return false;

	_configStore[plugin] = config;

	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;
	auto pagesVariable = a_configScript->GetProperty("Pages");

	if (vm && pagesVariable)
	{
		ScriptArrayPtr pagesArray;
		vm->CreateArray(
			RE::BSScript::TypeInfo{ RE::BSScript::TypeInfo::RawType::kString },
			static_cast<std::uint32_t>(config->SubPages.size()),
			pagesArray);

		std::uint32_t index = 0;
		for (auto& [pageDisplayName, pageContent] : config->SubPages)
		{
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
	if (it != _configStore.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

auto ConfigStore::GetConfig(RE::TESQuest* a_configQuest) -> std::shared_ptr<Config>
{
	auto modName = Utils::GetModName(a_configQuest);
	return GetConfig(modName);
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
	return Utils::GetModName(quest);
}
