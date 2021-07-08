#include "Config.h"
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
