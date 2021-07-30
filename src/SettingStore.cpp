#include "SettingStore.h"
#include "ColorUtil.h"
#include <SimpleIni.h>

SettingStore::SettingStore()
{
	logger::info("ModSettingStore initializing."sv);
}

SettingStore::~SettingStore()
{
	for (auto& [id, setting] : _settingStore) {
		delete[] setting->name;
		::free(setting);
	}
}

auto SettingStore::GetInstance() -> SettingStore&
{
	static SettingStore instance;
	return instance;
}

void SettingStore::ReadSettings()
{
	auto startTime = std::chrono::steady_clock::now();

	LoadDefaults();
	LoadUserSettings();

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	logger::info(
		"Registered {} mod settings in {} ms."sv,
		_settingStore.size(),
		elapsedMs.count());
}

auto SettingStore::GetModSettingInt(std::string_view a_modName, std::string_view a_settingName)
	-> std::int32_t
{
	auto setting = GetModSetting(a_modName, a_settingName);
	if (setting) {
		switch (setting->GetType()) {
		case RE::Setting::Type::kSignedInteger:
			return setting->data.i;
		case RE::Setting::Type::kUnsignedInteger:
			return setting->data.u;
		case RE::Setting::Type::kColor:
			return PackARGB(setting->data.r);
		default:
			return setting->data.i;
		}
	}
	return -1;
}

auto SettingStore::GetModSettingBool(std::string_view a_modName, std::string_view a_settingName)
	-> bool
{
	auto setting = GetModSetting(a_modName, a_settingName);
	return setting ? setting->data.b : false;
}

auto SettingStore::GetModSettingFloat(std::string_view a_modName, std::string_view a_settingName)
	-> float
{
	auto setting = GetModSetting(a_modName, a_settingName);
	return setting ? setting->data.f : -1;
}

auto SettingStore::GetModSettingString(std::string_view a_modName, std::string_view a_settingName)
	-> const char*
{
	auto setting = GetModSetting(a_modName, a_settingName);
	return setting ? setting->data.s : nullptr;
}

void SettingStore::SetModSettingInt(
	std::string_view a_modName,
	std::string_view a_settingName,
	std::int32_t a_newValue)
{
	auto setting = GetModSetting(a_modName, a_settingName);
	if (setting) {
		if (setting->GetType() == RE::Setting::Type::kColor) {
			auto color = UnpackARGB(static_cast<std::uint32_t>(a_newValue));
			if (setting->data.r != color) {
				setting->data.r = color;
				CommitModSetting(a_modName, setting);
			}
		}
		else {
			if (setting->data.i != a_newValue) {
				setting->data.i = a_newValue;
				CommitModSetting(a_modName, setting);
			}
		}
	}
}

void SettingStore::SetModSettingBool(
	std::string_view a_modName,
	std::string_view a_settingName,
	bool a_newValue)
{
	auto setting = GetModSetting(a_modName, a_settingName);
	if (setting && setting->data.b != a_newValue) {
		setting->data.b = a_newValue;
		CommitModSetting(a_modName, setting);
	}
}

void SettingStore::SetModSettingFloat(
	std::string_view a_modName,
	std::string_view a_settingName,
	float a_newValue)
{
	auto setting = GetModSetting(a_modName, a_settingName);
	if (setting && setting->data.f != a_newValue) {
		setting->data.f = a_newValue;
		CommitModSetting(a_modName, setting);
	}
}

void SettingStore::SetModSettingString(
	std::string_view a_modName,
	std::string_view a_settingName,
	std::string_view a_newValue)
{
	auto setting = GetModSetting(a_modName, a_settingName);
	if (setting && strcmp(setting->data.s, a_newValue.data()) != 0) {
		if (setting->data.s)
			delete[] setting->data.s;

		setting->data.s = new char[a_newValue.size() + 1];
		std::copy(a_newValue.begin(), a_newValue.end(), setting->data.s);
		setting->data.s[a_newValue.size()] = '\0';

		CommitModSetting(a_modName, setting);
	}
}

void SettingStore::ReloadDefault(std::string_view a_modName, std::string_view a_settingName)
{
	std::filesystem::path configPath{ "Data/MCM/Config"sv };
	auto iniLocation = configPath / a_modName / "settings.ini"sv;

	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error rc = ini.LoadFile(iniLocation.c_str());
	if (rc < 0) {
		logger::warn("Failed to parse file: {}"sv, iniLocation.string());
		return;
	}

	std::string modSettingName{ a_settingName };
	auto delimiter = modSettingName.find_first_of(':');
	if (delimiter != std::string::npos) {
		std::string settingName = modSettingName.substr(0, delimiter);
		std::string sectionName = modSettingName.substr(delimiter + 1);
		std::string value = ini.GetValue(sectionName.c_str(), settingName.c_str());

		RegisterModSetting(a_modName, a_settingName, value, _settingStore);
		CommitModSetting(a_modName, GetModSetting(a_modName, a_settingName));
	}
}

void SettingStore::ResetToDefault(std::string_view a_modName, std::string_view a_settingName)
{
	auto key = GetKey(a_modName, a_settingName);
	auto item = _settingStore.find(key);
	if (item == _settingStore.end())
		return;

	auto defaultItem = _defaults.find(key);
	if (defaultItem == _defaults.end())
		return;

	auto setting = item->second;
	auto& defaultValue = defaultItem->second->data;

	switch (setting->GetType()) {
	case RE::Setting::Type::kBool:
		setting->data.b = defaultValue.b;
		break;
	case RE::Setting::Type::kFloat:
		setting->data.f = defaultValue.f;
		break;
	case RE::Setting::Type::kSignedInteger:
		setting->data.i = defaultValue.i;
		break;
	case RE::Setting::Type::kColor:
		setting->data.r = defaultValue.r;
		break;
	case RE::Setting::Type::kString:
	{
		delete[] setting->data.s;
		auto bufSize = strlen(defaultValue.s) + 1;
		setting->data.s = new char[bufSize];
		strcpy_s(setting->data.s, bufSize, defaultValue.s);
		break;
	}
	case RE::Setting::Type::kUnsignedInteger:
		setting->data.u = defaultValue.u;
		break;
	default:
		return;
	}

	CommitModSetting(a_modName, setting);
}

bool SettingStore::ReadINI(
	std::string_view a_modName,
	const std::filesystem::path& a_iniLocation,
	bool a_isDefault)
{
	CSimpleIniA ini;
	ini.SetUnicode();

	SI_Error rc = ini.LoadFile(a_iniLocation.c_str());
	if (rc < 0) {
		logger::warn("Failed to parse file: {}"sv, a_iniLocation.string());
		return false;
	}

	std::list<CSimpleIniA::Entry> sections;
	ini.GetAllSections(sections);

	for (auto& section : sections) {
		std::string sectionName{ section.pItem };

		std::list<CSimpleIniA::Entry> keys;
		ini.GetAllKeys(section.pItem, keys);

		for (auto& key : keys) {
			std::string keyName{ key.pItem };

			auto settingName = keyName + ":" + sectionName;
			auto settingValue = ini.GetValue(section.pItem, key.pItem);

			if (a_isDefault) {
				RegisterModSetting(a_modName, settingName, settingValue, _defaults);
			}

			RegisterModSetting(a_modName, settingName, settingValue, _settingStore);
		}
	}

	return true;
}

auto SettingStore::GetModSetting(std::string_view a_modName, std::string_view a_settingName)
	-> RE::Setting*
{
	if (a_modName.empty() || a_settingName.empty())
		return nullptr;

	auto key = GetKey(a_modName, a_settingName);
	auto it = _settingStore.find(key);
	return (it != _settingStore.end()) ? it->second : nullptr;
}

auto SettingStore::GetDefaultSetting(std::string_view a_modName, std::string_view a_settingName)
	-> RE::Setting*
{
	if (a_modName.empty() || a_settingName.empty())
		return nullptr;

	auto key = GetKey(a_modName, a_settingName);
	auto it = _defaults.find(key);
	return (it != _defaults.end()) ? it->second : nullptr;
}

void SettingStore::LoadDefaults()
{
	std::filesystem::path configPath{ "Data/MCM/Config"sv };
	std::filesystem::directory_entry configDirectory{ configPath };
	if (!configDirectory.exists() || !configDirectory.is_directory())
		return;

	for (auto& entry : std::filesystem::directory_iterator{ configPath }) {
		if (!entry.is_directory())
			continue;

		auto modName = entry.path().stem().string();
		auto iniPath = entry.path() / "settings.ini"sv;
		std::filesystem::directory_entry iniEntry{ iniPath };

		if (iniEntry.exists() && iniEntry.is_regular_file()) {
			ReadINI(modName, iniPath, true);
		}
	}
}

void SettingStore::LoadUserSettings()
{
	std::filesystem::path modSettingsPath{ "Data/MCM/Settings"sv };
	std::filesystem::directory_entry modSettingsDirectory{ modSettingsPath };
	if (!modSettingsDirectory.exists() || !modSettingsDirectory.is_directory())
		return;

	std::size_t count = 0;
	for (auto& entry : std::filesystem::directory_iterator{ modSettingsPath }) {
		if (!entry.is_regular_file() || entry.path().extension() != ".ini"sv)
			continue;

		auto& iniPath = entry.path();
		auto modName = iniPath.stem().string();

		ReadINI(modName, iniPath, false);
		count++;
	}

	logger::info("Number of mod setting files: {}"sv, count);
}

void SettingStore::RegisterModSetting(
	std::string_view a_modName,
	std::string_view a_settingName,
	std::string_view a_settingValue,
	Storage& a_settingStore)
{
	auto newSetting = static_cast<RE::Setting*>(::malloc(sizeof(RE::Setting)));

	char* nameCopy = new char[a_settingName.size() + 1];
	std::copy(a_settingName.begin(), a_settingName.end(), nameCopy);
	nameCopy[a_settingName.size()] = '\0';
	newSetting->name = nameCopy;

	std::istringstream ssValue{ std::string{ a_settingValue } };

	switch (newSetting->GetType()) {
	case RE::Setting::Type::kSignedInteger:
		ssValue >> newSetting->data.i;
		break;

	case RE::Setting::Type::kFloat:
		ssValue >> newSetting->data.f;
		break;

	case RE::Setting::Type::kBool:
		std::uint32_t bValue;
		ssValue >> bValue;
		newSetting->data.b = static_cast<bool>(bValue);
		break;

	case RE::Setting::Type::kUnsignedInteger:
		ssValue >> newSetting->data.u;
		break;

	case RE::Setting::Type::kColor:
		ssValue >> newSetting->data.r;
		break;

	case RE::Setting::Type::kString:
		newSetting->data.s = new char[a_settingValue.size() + 1];
		std::copy(a_settingValue.begin(), a_settingValue.end(), newSetting->data.s);
		newSetting->data.s[a_settingValue.size()] = '\0';
		break;

	default:
		logger::warn(
			"ModSetting {} from mod {} has an unknown type and cannot be registered."sv,
			a_settingName,
			a_modName);
		delete[] newSetting->name;
		::free(newSetting);
		return;
	}

	auto key = GetKey(a_modName, a_settingName);

	auto it = a_settingStore.find(key);
	if (it != a_settingStore.end()) {
		delete[] it->second->name;
		::free(it->second);
		a_settingStore.erase(it);
	}

	a_settingStore[key] = newSetting;
}

void SettingStore::CommitModSetting(std::string_view a_modName, RE::Setting* a_modSetting)
{
	std::string modSettingName{ a_modSetting->name };
	auto delimiter = modSettingName.find_first_of(':');
	if (delimiter != std::string::npos) {
		std::string settingName = modSettingName.substr(0, delimiter);
		std::string sectionName = modSettingName.substr(delimiter + 1);
		std::string value;

		switch (a_modSetting->GetType()) {
		case RE::Setting::Type::kSignedInteger:
			value = std::to_string(a_modSetting->data.i);
			break;
		case RE::Setting::Type::kFloat:
			value = std::to_string(a_modSetting->data.f);
			break;
		case RE::Setting::Type::kBool:
			value = std::to_string(a_modSetting->data.b);
			break;
		case RE::Setting::Type::kUnsignedInteger:
			value = std::to_string(a_modSetting->data.u);
			break;
		case RE::Setting::Type::kColor:
			value = std::to_string(a_modSetting->data.r);
			break;
		case RE::Setting::Type::kString:
			value = a_modSetting->data.s;
			break;
		default:
			logger::warn(
				"ModSetting {} from mod {} has an unknown type and cannot be saved."sv,
				settingName,
				a_modName);
			return;
		}

		std::filesystem::path settingsPath{ "Data/MCM/Settings"sv };
		std::filesystem::directory_entry settingsDirectory{ settingsPath };
		if (!settingsDirectory.exists()) {
			std::filesystem::create_directory(settingsPath);
		}

		std::filesystem::path iniPath = settingsPath / (std::string{ a_modName } + ".ini"s);
		std::filesystem::directory_entry iniFile{ iniPath };

		CSimpleIniA ini;
		ini.SetUnicode();

		if (iniFile.exists()) {
			ini.LoadFile(iniPath.c_str());
			SI_Error rc = ini.LoadFile(iniPath.c_str());
			if (rc < 0) {
				logger::warn("Failed to parse file: {}"sv, iniPath.string());
				return;
			}
		}

		ini.SetValue(sectionName.c_str(), settingName.c_str(), value.c_str());

		SI_Error rc = ini.SaveFile(iniPath.c_str());
		if (rc < 0) {
			logger::warn("Failed to save file: {}"sv, iniPath.string());
			return;
		}
	}
	else {
		logger::warn("Section could not be resolved."sv);
	}
}

std::string SettingStore::GetKey(std::string_view a_modName, std::string_view a_settingName)
{
	return std::string{ a_modName } + ":" + std::string{ a_settingName };
}