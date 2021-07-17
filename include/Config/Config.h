#pragma once

#include "Config/PageContent.h"

class Config
{
public:
	void ShowPage(ScriptObjectPtr a_object, const std::string& a_page = ""s) const;
	void RefreshPage(ScriptObjectPtr a_object) const;
	auto GetCustomControl(std::int32_t a_keyCode) const -> std::string;

	std::shared_ptr<PageContent> MainPage;
	tsl::ordered_map<std::string, std::shared_ptr<PageContent>> SubPages;
};