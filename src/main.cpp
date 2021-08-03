#include "Papyrus/RegisterFuncs.h"
#include "SettingStore.h"
#include "ConfigStore.h"
#include "KeybindEventHandler.h"
#include "KeybindManager.h"

extern "C" DLLEXPORT bool SKSEAPI
	SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= "MCMHelper.log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::warn);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);

	logger::info("MCMHelper v1.0.3"sv);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = "MCMHelper";
	a_info->version = 1;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
#ifndef SKYRIMVR
	if (ver < SKSE::RUNTIME_1_5_39) {
#else
	if (ver != SKSE::RUNTIME_VR_1_4_15_1) {
#endif
		logger::critical(FMT_STRING("Unsupported runtime version {}"sv), ver.string());
		return false;
	}

	return true;
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("MCMHelper loaded"sv);

	SKSE::Init(a_skse);

	SKSE::GetPapyrusInterface()->Register(Papyrus::RegisterFuncs);

	std::filesystem::path mcmPath{ "Data/MCM"sv };
	std::filesystem::directory_entry mcmDirectory{ mcmPath };
	if (!mcmDirectory.exists()) {
		std::filesystem::create_directory(mcmPath);
	}

	SettingStore::GetInstance().ReadSettings();
	KeybindManager::GetInstance().ReadKeybindRegistrations();

	SKSE::GetMessagingInterface()->RegisterListener(
		[](SKSE::MessagingInterface::Message* a_msg)
		{
			switch (a_msg->type) {
			case SKSE::MessagingInterface::kPostLoadGame:
				KeybindManager::GetInstance().ClearKeybinds();
				ConfigStore::GetInstance().ReadConfigs();
				break;
			case SKSE::MessagingInterface::kInputLoaded:
				KeybindEventHandler::GetInstance().Register();
				break;
			}
		});

	return true;
}
