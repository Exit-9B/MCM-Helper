#pragma once

class SettingStore
{
	using Storage = std::unordered_map<std::string, RE::Setting*>;

public:
	~SettingStore();
	SettingStore(const SettingStore&) = delete;
	SettingStore(SettingStore&&) = delete;
	SettingStore& operator=(const SettingStore&) = delete;
	SettingStore& operator=(SettingStore&&) = delete;

	static SettingStore& GetInstance();

	void ReadSettings();

	auto GetModSettingInt(
		std::string_view a_modName,
		std::string_view a_settingName) -> std::int32_t;

	auto GetModSettingBool(
		std::string_view a_modName,
		std::string_view a_settingName) -> bool;

	auto GetModSettingFloat(
		std::string_view a_modName,
		std::string_view a_settingName) -> float;

	auto GetModSettingString(
		std::string_view a_modName,
		std::string_view a_settingName) -> const char*;

	void SetModSettingInt(
		std::string_view a_modName,
		std::string_view a_settingName,
		std::int32_t a_newValue);

	void SetModSettingBool(
		std::string_view a_modName,
		std::string_view a_settingName,
		bool a_newValue);

	void SetModSettingFloat(
		std::string_view a_modName,
		std::string_view a_settingName,
		float a_newValue);

	void SetModSettingString(
		std::string_view a_modName,
		std::string_view a_settingName,
		std::string_view a_newValue);

	void ReloadDefault(
		std::string_view a_modName,
		std::string_view a_settingName);

	void ResetToDefault(
		std::string_view a_modName,
		std::string_view a_settingName);

	auto GetModSetting(
		std::string_view a_modName,
		std::string_view a_settingName)
		-> RE::Setting*;

	auto GetDefaultSetting(
		std::string_view a_modName,
		std::string_view a_settingName)
		-> RE::Setting*;

private:
	SettingStore();

	bool ReadINI(
		std::string_view a_modName,
		const std::filesystem::path& a_iniLocation,
		bool a_isDefault);

	void LoadDefaults();

	void LoadUserSettings();

	void RegisterModSetting(
		std::string_view a_modName,
		std::string_view a_settingName,
		std::string_view a_settingValue,
		Storage& a_settingStore);

	void CommitModSetting(
		std::string_view a_modName,
		RE::Setting* a_modSetting);

	auto GetKey(
		std::string_view a_modName,
		std::string_view a_settingName)
		-> std::string;

	Storage _defaults;
	Storage _settingStore;
};