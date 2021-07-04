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
	const std::vector<std::string>& a_options)
{
	_menuOptions[a_ID] = a_options;
}

auto ConfigPageCache::GetCurrentForm() const -> RE::TESForm*
{
	return _currentForm;
}

auto ConfigPageCache::GetCurrentScriptName() const -> const char*
{
	return !_currentScript.empty() ? _currentScript.c_str() : nullptr;
}

auto ConfigPageCache::GetGroupFlags(std::uint32_t a_groupID) const -> SkyUI::Flags
{
	auto it = _groupControls.find(a_groupID);
	auto control = it != _groupControls.end() ? it->second : nullptr;
	if (control && !control->GetValue())
	{
		return control->GroupMode;
	}

	return SkyUI::Flags::None;
}

auto ConfigPageCache::GetPropertyVariable(
	RE::TESForm* a_form,
	const std::string& a_scriptName,
	const std::string& a_propertyName)
	-> RE::BSScript::Variable* const
{
	RE::TESForm* form = a_form;
	const char* scriptName = !a_scriptName.empty() ? a_scriptName.c_str() : nullptr;
	if (!form)
	{
		form = a_form ? a_form : _currentForm;
		if (a_scriptName.empty())
		{
			scriptName = _currentScript.c_str();
		}
	}

	auto script = Utils::GetScriptObject(form, scriptName);
	return script ? script->GetProperty(a_propertyName) : nullptr;
}

auto ConfigPageCache::GetControl(std::int32_t a_optionID) const -> std::shared_ptr<Control>
{
	auto it = _pageCache.find(a_optionID);
	return it != _pageCache.end() ? it->second : nullptr;
}

auto ConfigPageCache::GetMenuOptions(std::shared_ptr<MenuControl> a_control) const
	-> std::vector<std::string>
{
	if (!a_control)
		return std::vector<std::string>{};

	auto& id = a_control->ID;
	auto it = _menuOptions.find(id);
	return it != _menuOptions.end() ? it->second : a_control->Options;
}

void ConfigPageCache::ForEach(
	std::function<void(std::int32_t, std::shared_ptr<Control>)> a_func) const
{
	for (auto& [id, control] : _pageCache)
	{
		a_func(id, control);
	}
}