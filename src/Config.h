#pragma once

#include "PageContent.h"

class Config
{
public:
	void ShowPage(ScriptObjectPtr a_object, std::string_view a_page = ""sv) const;
	void RefreshPage(ScriptObjectPtr a_object) const;

	std::shared_ptr<PageContent> MainPage;
	tsl::ordered_map<std::string_view, std::shared_ptr<PageContent>> SubPages;
};