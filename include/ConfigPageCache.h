#pragma once

#include "Config/Config.h"
#include "Script/SkyUI.h"

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

	// Set data

	void ClearPageCache();

	void ClearMenuOptions();

	void SetCurrentScript(RE::TESForm* a_form, const std::string& a_scriptName);

	void SetGroupControls(
		const std::unordered_map<std::uint32_t, std::shared_ptr<ToggleControl>>& a_controls);

	void AddControl(std::int32_t a_optionID, std::shared_ptr<Control> a_control);

	void SetMenuOptions(
		const std::string& a_ID,
		const std::vector<std::string>& a_options,
		const std::vector<std::string>& a_shortNames);

	void SetHighlight(std::int32_t a_option);

	// Get data

	auto GetCurrentForm() const -> RE::TESForm*;

	auto GetCurrentScriptName() const -> const std::string&;

	auto GetGroups() const -> std::unordered_set<std::uint32_t>;

	auto IsGroupActive(std::uint32_t a_groupID) const -> bool;

	auto GetControl(std::int32_t a_optionID) const -> std::shared_ptr<Control>;

	auto GetMenuOptions(MenuDialogControl* a_control) const -> std::vector<std::string>;

	auto GetMenuShortNames(MenuDialogControl* a_control) const -> std::vector<std::string>;

	auto GetHighlightedControl() const -> std::shared_ptr<Control>;

	void ForEach(std::function<void(std::int32_t, std::shared_ptr<Control>)> a_func) const;

private:
	ConfigPageCache() = default;

	// Config
	RE::TESForm* _currentForm;
	std::string _currentScript;
	std::unordered_map<std::string, std::vector<std::string>> _menuOptions;
	std::unordered_map<std::string, std::vector<std::string>> _menuShortNames;

	// Page
	std::unordered_map<std::int32_t, std::shared_ptr<Control>> _pageCache;
	std::unordered_map<std::uint32_t, std::shared_ptr<ToggleControl>> _groupControls;
	std::int32_t _highlight = -1;
};
