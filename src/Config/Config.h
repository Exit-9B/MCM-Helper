#pragma once

#include "Config/PageContent.h"

class Config
{
public:
	void ShowPage(const ScriptObjectPtr& a_object, const std::string& a_page = ""s) const;
	void RefreshPage(const ScriptObjectPtr& a_object) const;
	[[nodiscard]] std::string GetCustomControl(std::uint32_t a_keyCode) const;

	std::shared_ptr<PageContent> MainPage;
	tsl::ordered_map<std::string, std::shared_ptr<PageContent>> SubPages;
};
