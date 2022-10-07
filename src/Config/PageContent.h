#pragma once

#include "Config/Control.h"
#include "Script/SkyUI.h"

class PageContent
{
public:
	virtual void Draw(const ScriptObjectPtr& a_object) const = 0;
};

class PageLayout : public PageContent
{
public:
	virtual void Draw(const ScriptObjectPtr& a_object) const override;

	SkyUI::CursorFillMode CursorFillMode = SkyUI::CursorFillMode::LeftToRight;
	std::vector<std::shared_ptr<Control>> Controls;
	std::unordered_map<std::uint32_t, std::shared_ptr<ToggleControl>> GroupControls;
};

class CustomContent : public PageContent
{
public:
	virtual void Draw(const ScriptObjectPtr& a_object) const override;

	std::string Source;
	float X = 0.0f;
	float Y = 0.0f;
};
