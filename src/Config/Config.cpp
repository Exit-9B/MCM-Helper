#include "Config/Config.h"
#include "ConfigPageCache.h"

void Config::ShowPage(ScriptObjectPtr a_object, const std::string& a_page) const
{
	if (a_page.empty())
	{
		if (MainPage)
		{
			MainPage->Draw(a_object);
		}
	}
	else
	{
		auto it = SubPages.find(a_page);
		if (it != SubPages.end())
		{
			auto& content = it->second;

			if (content)
			{
				content->Draw(a_object);
			}
		}
	}
}

void Config::RefreshPage(ScriptObjectPtr a_object) const
{
	ConfigPageCache::GetInstance().ForEach(
		[&a_object](std::int32_t a_ID, std::shared_ptr<Control> a_control)
		{
			a_control->Refresh(a_object, a_ID);
		});
}

auto Config::GetCustomControl(std::int32_t a_keyCode) const -> std::string
{
	if (auto pageLayout = std::dynamic_pointer_cast<PageLayout>(MainPage))
	{
		for (auto& control : pageLayout->Controls)
		{
			auto keymap = std::dynamic_pointer_cast<KeyMapControl>(control);
			if (keymap && !keymap->IgnoreConflicts && keymap->GetKeyCode() == a_keyCode)
			{
				return keymap->Text;
			}
		}
	}

	for (auto& [name, page] : SubPages)
	{
		auto pageLayout = std::dynamic_pointer_cast<PageLayout>(page);
		if (!pageLayout)
			continue;

		for (auto& control : pageLayout->Controls)
		{
			auto keymap = std::dynamic_pointer_cast<KeyMapControl>(control);
			if (keymap && !keymap->IgnoreConflicts && keymap->GetKeyCode() == a_keyCode)
			{
				return keymap->Text;
			}
		}
	}

	return ""s;
}
