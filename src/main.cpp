#include "Papyrus/RegisterFuncs.h"
#include "SettingStore.h"
#include "ConfigStore.h"
#include "KeybindEventHandler.h"
#include "KeybindManager.h"

void InitLogger()
{
	static bool initialized = false;
	if (!initialized) {
		initialized = true;
	}
	else {
		return;
	}

#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return;
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
}

#ifndef SKYRIMVR

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version =
[]() {
	SKSE::PluginVersionData v{};
	v.pluginVersion = Version::MAJOR;
	v.PluginName(Version::PROJECT);
	v.AuthorName("Parapets"sv);
	v.UsesAddressLibrary(true);
	return v;
}();

#else

extern "C" DLLEXPORT bool SKSEAPI
	SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	InitLogger();

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
	InitLogger();

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
