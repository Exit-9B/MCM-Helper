#include "Json/ConfigHandler.h"
#include "Json/PluginRequirementsHandler.h"
#include "Json/ContentHandler.h"
#include "Json/CustomContentHandler.h"
#include "Json/PagesHandler.h"

ConfigHandler::ConfigHandler(
	ReaderHandler* master,
	Config* config,
	const std::string& modName,
	const ScriptObjectPtr& script) :
	_master{ master },
	_config{ config },
	_modName{ modName },
	_script{ script }
{
}

bool ConfigHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::MinMcmVersion:
		return PLUGIN_VERSION >= i;
	default:
		return false;
	}
}

bool ConfigHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::ModName:
		_state = State::Start;
		return strcmp(str, _modName.c_str()) == 0;
	case State::DisplayName:
	{
		auto variable = _script->GetProperty("ModName"sv);
		if (variable) {
			variable->SetString(str);
		}
		_state = State::Start;
		return true;
	}
	case State::CursorFillMode:
		if (strcmp(str, "leftToRight") == 0) {
			_cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
			_state = State::Start;
			return true;
		}
		else if (strcmp(str, "topToBottom") == 0) {
			_cursorFillMode = SkyUI::CursorFillMode::TopToBottom;
			_state = State::Start;
			return true;
		}
		else {
			return false;
		}
	default:
		return false;
	}
}

bool ConfigHandler::StartObject()
{
	switch (_state) {
	case State::End:
		_state = State::Start;
		return true;
	default:
		return false;
	}
}

bool ConfigHandler::Key(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Start:
		if (strcmp(str, "modName") == 0) {
			_state = State::ModName;
			return true;
		}
		else if (strcmp(str, "displayName") == 0) {
			_state = State::DisplayName;
			return true;
		}
		else if (strcmp(str, "minMcmVersion") == 0) {
			_state = State::MinMcmVersion;
			return true;
		}
		else if (strcmp(str, "pluginRequirements") == 0) {
			_master->PushHandler<PluginRequirementsHandler>(_master);
			return true;
		}
		else if (strcmp(str, "content") == 0) {
			auto pageLayout = std::make_shared<PageLayout>();
			_config->MainPage = pageLayout;
			_master->PushHandler<ContentHandler>(_master, pageLayout.get(), _script);
			return true;
		}
		else if (strcmp(str, "customContent") == 0) {
			auto customContent = std::make_shared<CustomContent>();
			_config->MainPage = customContent;
			_master->PushHandler<CustomContentHandler>(_master, customContent.get());
			return true;
		}
		else if (strcmp(str, "cursorFillMode") == 0) {
			_state = State::CursorFillMode;
			return true;
		}
		else if (strcmp(str, "pages") == 0) {
			_master->PushHandler<PagesHandler>(_master, _config, _script);
			return true;
		}
		else {
			return false;
		}
	default:
		return false;
	}
}

bool ConfigHandler::EndObject([[maybe_unused]] SizeType memberCount)
{
	switch (_state) {
	case State::Start:
		if (auto pageLayout = std::dynamic_pointer_cast<PageLayout>(_config->MainPage)) {
			pageLayout->CursorFillMode = _cursorFillMode;
		}
		_state = State::End;
		return true;
	default:
		return false;
	}
}
