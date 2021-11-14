#include "Papyrus/RegisterFuncs.h"
#include "SettingStore.h"
#include "ConfigStore.h"
#include "KeybindEventHandler.h"
#include "KeybindManager.h"

#ifndef SKYRIMVR

extern "C" DLLEXPORT SKSE::PluginVersionData SKSEPlugin_Version =
{
	.dataVersion = SKSE::PluginVersionData::kVersion,

	.pluginVersion = 1,
	.name = PROJECT_NAME,

	.author = "Parapets",
	.supportEmail = "",

	.versionIndependence = 0,
	.compatibleVersions = { SKSE::RUNTIME_1_6_318.packed(), 0 },

	.seVersionRequired = 0,
};

#else

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

	*path /= fmt::format("{}.log"sv, Version::PROJECT);
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

	logger::info("{} v{}"sv, Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver != SKSE::RUNTIME_VR_1_4_15_1) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"sv), ver.string());
		return false;
	}

	return true;
}

#endif


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("{} loaded"sv, Version::PROJECT);

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
				KeybindManager::GetInstance().ClearModKeys();
				ConfigStore::GetInstance().ReadConfigs();
				break;
			case SKSE::MessagingInterface::kInputLoaded:
				KeybindEventHandler::GetInstance().Register();
				break;
			case SKSE::MessagingInterface::kNewGame:
				ConfigStore::GetInstance().ClearConfigs();
				KeybindManager::GetInstance().ClearModKeys();
				break;
			}
		});

	spdlog::default_logger()->flush();

	return true;
}
