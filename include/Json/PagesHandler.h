#pragma once

#include "Json/IHandler.h"
#include "Config/Config.h"
#include "Config/PageContent.h"
#include "Script/SkyUI.h"

class PagesHandler : public IHandler
{
public:
	PagesHandler(ReaderHandler* master, Config* config, const ScriptObjectPtr& script);

	bool String(const Ch* str, SizeType length, bool copy);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);
	bool StartArray();
	bool EndArray(SizeType elementCount);

private:
	enum class State
	{
		Init,
		Main,
		Page,
		PageDisplayName,
		CursorFillMode,
	};

	State _state = State::Init;
	ScriptObjectPtr _script;

	Config* _config;
	std::string _pageDisplayName;
	SkyUI::CursorFillMode _cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
	std::shared_ptr<PageContent> _pageContent;
};
