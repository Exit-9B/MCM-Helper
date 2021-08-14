#include "ConfigPageCache.h"

auto ConfigPageCache::GetInstance() -> ConfigPageCache&
{
	static ConfigPageCache instance;
	return instance;
}

void ConfigPageCache::ClearCache()
{
	_currentForm = nullptr;
	_currentScript = ""s;
	_pageCache.clear();
	_menuOptions.clear();
	_menuShortNames.clear();
	_groupControls.clear();
}

void ConfigPageCache::SetCurrentScript(RE::TESForm* a_form, const std::string& a_scriptName)
{
	_currentForm = a_form;
	_currentScript = a_scriptName;
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

auto ConfigPageCache::GetCurrentForm() const -> RE::TESForm*
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

auto ConfigPageCache::IsGroupActive(std::uint32_t a_groupID) const -> bool
{
	auto it = _groupControls.find(a_groupID);
	auto control = it != _groupControls.end() ? it->second : nullptr;
	return control ? control->GetValue() : true;
}

auto ConfigPageCache::GetControl(std::int32_t a_optionID) const -> std::shared_ptr<Control>
{
	auto it = _pageCache.find(a_optionID);
	return it != _pageCache.end() ? it->second : nullptr;
}

auto ConfigPageCache::GetMenuOptions(MenuDialogControl* a_control) const
	-> std::vector<std::string>
{
	if (!a_control)
		return std::vector<std::string>{};

	auto& id = a_control->ID;
	auto it = _menuOptions.find(id);
	return it != _menuOptions.end() ? it->second : a_control->Options;
}

auto ConfigPageCache::GetMenuShortNames(MenuDialogControl* a_control) const
	-> std::vector<std::string>
{
	if (!a_control)
		return std::vector<std::string>{};

	auto& id = a_control->ID;
	auto it = _menuShortNames.find(id);
	return it != _menuShortNames.end() ? it->second : a_control->ShortNames;
}

void ConfigPageCache::ForEach(
	std::function<void(std::int32_t, std::shared_ptr<Control>)> a_func) const
{
	for (auto& [id, control] : _pageCache) {
		a_func(id, control);
	}
}
