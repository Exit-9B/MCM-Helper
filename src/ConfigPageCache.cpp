#include "ConfigPageCache.h"

ConfigPageCache& ConfigPageCache::GetInstance()
{
	static ConfigPageCache instance{};
	return instance;
}

void ConfigPageCache::ClearPageCache()
{
	_pageCache.clear();
	_groupControls.clear();
	_highlight = -1;
}

void ConfigPageCache::ClearMenuOptions()
{
	_menuOptions.clear();
	_menuShortNames.clear();
}

void ConfigPageCache::SetCurrentScript(RE::TESForm* a_form, const std::string& a_scriptName)
{
	if (a_form == _currentForm && a_scriptName == _currentScript)
		return;

	_currentForm = a_form;
	_currentScript = a_scriptName;

	ClearMenuOptions();
	ClearPageCache();
}

void ConfigPageCache::SetGroupControls(
	const std::unordered_map<std::uint32_t, std::shared_ptr<ToggleControl>>& a_controls)
{
	_groupControls = a_controls;
}

void ConfigPageCache::AddControl(std::int32_t a_optionID, std::shared_ptr<Control> a_control)
{
	_pageCache[a_optionID] = a_control;
}

void ConfigPageCache::SetMenuOptions(
	const std::string& a_ID,
	const std::vector<std::string>& a_options,
	const std::vector<std::string>& a_shortNames)
{
	_menuOptions[a_ID] = a_options;
	_menuShortNames[a_ID] = a_shortNames;
}

void ConfigPageCache::SetHighlight(std::int32_t a_option)
{
	_highlight = a_option;
}

RE::TESForm* ConfigPageCache::GetCurrentForm() const
{
	return _currentForm;
}

auto ConfigPageCache::GetCurrentScriptName() const -> const std::string&
{
	return _currentScript;
}

auto ConfigPageCache::GetGroups() const -> std::unordered_set<std::uint32_t>
{
	std::unordered_set<std::uint32_t> groups;
	for (auto& [group, control] : _groupControls) {
		groups.insert(group);
	}

	return groups;
}

bool ConfigPageCache::IsGroupActive(std::uint32_t a_groupID) const
{
	if (auto it = _groupControls.find(a_groupID); it != _groupControls.end()) {
		if (auto& control = it->second) {
			return control->GetValue();
		}
	}

	return true;
}

auto ConfigPageCache::GetControl(std::int32_t a_optionID) const -> std::shared_ptr<Control>
{
	auto it = _pageCache.find(a_optionID);
	return it != _pageCache.end() ? it->second : nullptr;
}

auto ConfigPageCache::GetMenuOptions(const MenuDialogControl* a_control) const
	-> std::vector<std::string>
{
	if (!a_control)
		return {};

	auto& id = a_control->ID;
	auto it = _menuOptions.find(id);
	return it != _menuOptions.end() ? it->second : a_control->Options;
}

auto ConfigPageCache::GetMenuShortNames(const MenuDialogControl* a_control) const
	-> std::vector<std::string>
{
	if (!a_control)
		return {};

	auto& id = a_control->ID;
	auto it = _menuShortNames.find(id);
	return it != _menuShortNames.end() ? it->second : a_control->ShortNames;
}

auto ConfigPageCache::GetHighlightedControl() const -> std::shared_ptr<Control>
{
	return GetControl(_highlight);
}

void ConfigPageCache::ForEach(
	std::function<void(std::int32_t, std::shared_ptr<Control>)> a_func) const
{
	for (auto& [id, control] : _pageCache) {
		a_func(id, control);
	}
}
