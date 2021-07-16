#pragma once

#include <rapidjson/reader.h>
#include "Json/PluginRequirementsHandler.h"
#include "Json/ContentHandler.h"
#include "Json/CustomContentHandler.h"
#include "Json/PagesHandler.h"
#include "SkyUI.h"

class ConfigHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ConfigHandler>
{
public:
	using SizeType = rapidjson::SizeType;

	ConfigHandler(
		Config* config,
		const std::string& modName,
		const ScriptObjectPtr& script);

	bool Null();
	bool Bool(bool b);
	bool Int(int i);
	bool Uint(unsigned i);
	bool Int64(std::int64_t i);
	bool Uint64(std::uint64_t i);
	bool Double(double d);
	bool RawNumber(const Ch* str, SizeType length, bool copy);
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
		ModName,
		DisplayName,
		MinMcmVersion,
		PluginRequirements,
		Content,
		CustomContent,
		CursorFillMode,
		Pages,
	};

	State _state = State::End;

	Config* _config;
	std::string _modName;
	ScriptObjectPtr _script;

	SkyUI::CursorFillMode _cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
	std::unique_ptr<PluginRequirementsHandler> _pluginRequirements;
	std::unique_ptr<ContentHandler> _content;
	std::unique_ptr<CustomContentHandler> _customContent;
	std::unique_ptr<PagesHandler> _pages;
};
