#pragma once

#include <rapidjson/reader.h>
#include "Json/ContentHandler.h"
#include "Json/CustomContentHandler.h"
#include "Config.h"
#include "PageContent.h"
#include "SkyUI.h"

class PagesHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	PagesHandler(Config* config, const ScriptObjectPtr& script);

	bool Complete();

	bool Bool(bool b);
	bool Int(int i);
	bool Uint(unsigned i);
	bool Double(double d);
	bool String(const Ch* str, SizeType length, bool copy);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);
	bool StartArray();
	bool EndArray(SizeType elementCount);

private:
	enum class State
	{
		End,
		Start,
		Page,
		PageDisplayName,
		Content,
		CustomContent,
		CursorFillMode,
	};

	State _state = State::End;
	ScriptObjectPtr _script;

	Config* _config;
	std::string _pageDisplayName;
	SkyUI::CursorFillMode _cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
	std::shared_ptr<PageContent> _pageContent;
	std::unique_ptr<ContentHandler> _content;
	std::unique_ptr<CustomContentHandler> _customContent;
};
