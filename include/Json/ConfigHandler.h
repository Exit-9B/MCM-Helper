#pragma once

#include "Json/IHandler.h"
#include "Config/Config.h"
#include "Script/SkyUI.h"

class ConfigHandler : public IHandler
{
public:
	ConfigHandler(
		ReaderHandler* master,
		Config* config,
		const std::string& modName,
		const ScriptObjectPtr& script);

	bool Uint(unsigned i);
	bool String(const Ch* str, SizeType length, bool copy);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);

private:
	enum class State
	{
		Init,
		Main,
		ModName,
		DisplayName,
		MinMcmVersion,
		CursorFillMode,
	};

	State _state = State::Init;

	Config* _config;
	std::string _modName;
	ScriptObjectPtr _script;

	bool _hasModName = false;
	bool _hasDisplayName = false;
	SkyUI::CursorFillMode _cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
};
