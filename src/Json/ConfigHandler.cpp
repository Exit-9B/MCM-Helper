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
	IHandler(master),
	_config{ config },
	_modName{ modName },
	_script{ script }
{
}

bool ConfigHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::MinMcmVersion:
		_state = State::Main;
		if (i > MCM_VERSION_RELEASE) {
			return ReportError("Config requires {} plugin version: {}"sv, Plugin::NAME, i);
		}
		return true;
	default:
		return IHandler::Uint(i);
	}
}

bool ConfigHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Schema:
		// Ignored
		_state = State::Main;
		return true;
	case State::ModName:
		_hasModName = true;
		_state = State::Main;
		if (str != _modName) {
			return ReportError("modName: \"{}\" did not match plugin"sv, str);
		}
		return true;
	case State::DisplayName:
	{
		_hasDisplayName = true;
		const auto variable = _script->GetProperty("ModName"sv);
		if (variable) {
			variable->SetString(str);
		}
		_state = State::Main;
		return true;
	}
	case State::CursorFillMode:
		if (strcmp(str, "leftToRight") == 0) {
			_cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
			_state = State::Main;
			return true;
		}
		else if (strcmp(str, "topToBottom") == 0) {
			_cursorFillMode = SkyUI::CursorFillMode::TopToBottom;
			_state = State::Main;
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidValue, str);
		}
	default:
		return IHandler::String(str, length, copy);
	}
}

bool ConfigHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool ConfigHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Main:
		if (strcmp(str, "$schema") == 0) {
			_state = State::Schema;
			return true;
		}
		else if (strcmp(str, "modName") == 0) {
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
			_master->PushHandler<PluginRequirementsHandler>();
			return true;
		}
		else if (strcmp(str, "content") == 0) {
			const auto pageLayout = std::make_shared<PageLayout>();
			_config->MainPage = pageLayout;
			_master->PushHandler<ContentHandler>(pageLayout.get(), _script);
			return true;
		}
		else if (strcmp(str, "customContent") == 0) {
			const auto customContent = std::make_shared<CustomContent>();
			_config->MainPage = customContent;
			_master->PushHandler<CustomContentHandler>(customContent.get());
			return true;
		}
		else if (strcmp(str, "cursorFillMode") == 0) {
			_state = State::CursorFillMode;
			return true;
		}
		else if (strcmp(str, "pages") == 0) {
			_master->PushHandler<PagesHandler>(_config, _script);
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool ConfigHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Main:
		if (const auto pageLayout = std::dynamic_pointer_cast<PageLayout>(_config->MainPage)) {
			pageLayout->CursorFillMode = _cursorFillMode;
		}

		if (!_hasModName) {
			return ReportError(ErrorType::MissingRequiredField, "modName"s);
		}

		if (!_hasDisplayName) {
			return ReportError(ErrorType::MissingRequiredField, "displayName"s);
		}

		return true;
	default:
		return IHandler::EndObject(memberCount);
	}
}
