#include "Papyrus/RegisterFuncs.h"
#include "SettingStore.h"
#include "ConfigStore.h"
#include "KeybindEventHandler.h"
#include "KeybindManager.h"

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);
	}
}

#ifndef SKYRIMVR

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version =
[]() {
	SKSE::PluginVersionData v{};

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("Parapets"sv);

	v.UsesAddressLibrary(true);
	v.HasNoStructUse(true);
	v.UsesStructsPost629(false);
	v.MinimumRequiredXSEVersion({ 2, 2, 5 });

	return v;
}();

#else

extern "C" DLLEXPORT bool SKSEAPI
	SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver != SKSE::RUNTIME_VR_1_4_15_1) {
		return false;
	}

	return true;
}

#endif


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

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
