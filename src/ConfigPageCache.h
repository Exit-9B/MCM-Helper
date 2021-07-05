#pragma once

#include "Config.h"
#include "SkyUI.h"

/* Stores data about the current MCM page and provides helpers for fetching data back
 */
class ConfigPageCache
{
public:
	~ConfigPageCache() = default;
	ConfigPageCache(const ConfigPageCache&) = delete;
	ConfigPageCache(ConfigPageCache&&) = delete;
	ConfigPageCache& operator=(const ConfigPageCache&) = delete;
	ConfigPageCache& operator=(ConfigPageCache&&) = delete;

	static auto GetInstance() -> ConfigPageCache&;

	void ClearCache();

	void SetCurrentScript(RE::TESForm* a_form, const std::string& a_scriptName);

	void SetGroupControls(
		const std::unordered_map<std::uint32_t, std::shared_ptr<ToggleControl>>& a_controls);

	void AddControl(std::int32_t a_optionID, std::shared_ptr<Control> a_control);

	void SetMenuOptions(const std::string& a_ID, const std::vector<std::string>& a_options);

	auto GetCurrentForm() const -> RE::TESForm*;

	auto GetCurrentScriptName() const -> const char*;

	auto GetGroupFlags(std::uint32_t a_groupID) const -> SkyUI::Flags;

	auto GetPropertyVariable(
		RE::TESForm* a_form,
		const std::string& a_scriptName,
		const std::string& a_propertyName)
		-> RE::BSScript::Variable* const;

	auto GetControl(std::int32_t a_optionID) const -> std::shared_ptr<Control>;

	auto GetMenuOptions(std::shared_ptr<MenuControl> a_control) const -> std::vector<std::string>;

	void ForEach(std::function<void(std::int32_t, std::shared_ptr<Control>)> a_func) const;

private:
	ConfigPageCache() = default;

	RE::TESForm* _currentForm;
	std::string _currentScript;
	std::unordered_map<std::int32_t, std::shared_ptr<Control>> _pageCache;
	std::unordered_map<std::string, std::vector<std::string>> _menuOptions;
	std::unordered_map<std::uint32_t, std::shared_ptr<ToggleControl>> _groupControls;
};