#include "Json/ConfigHandler.h"

ConfigHandler::ConfigHandler(
	Config* config,
	const std::string& modName,
	const ScriptObjectPtr& script) :
	_config{ config },
	_modName{ modName },
	_script{ script }
{
}

bool ConfigHandler::Null()
{
	return false;
}

bool ConfigHandler::Bool(bool b)
{
	switch (_state) {
	case State::Content:
		return _content->Bool(b);
	case State::Pages:
		return _pages->Bool(b);
	default:
		return false;
	}
}

bool ConfigHandler::Int(int i)
{
	switch (_state) {
	case State::Content:
		return _content->Int(i);
	case State::CustomContent:
		return _customContent->Int(i);
	case State::Pages:
		return _pages->Int(i);
	default:
		return false;
	}
}

bool ConfigHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::MinMcmVersion:
		return PLUGIN_VERSION >= i;
	case State::Content:
		return _content->Uint(i);
	case State::CustomContent:
		return _customContent->Uint(i);
	case State::Pages:
		return _pages->Uint(i);
	default:
		return false;
	}
}

bool ConfigHandler::Int64([[maybe_unused]] std::int64_t i)
{
	return false;
}

bool ConfigHandler::Uint64([[maybe_unused]] std::uint64_t i)
{
	return false;
}

bool ConfigHandler::Double(double d)
{
	switch (_state) {
	case State::Content:
		return _content->Double(d);
	case State::CustomContent:
		return _customContent->Double(d);
	case State::Pages:
		return _pages->Double(d);
	default:
		return false;
	}
}

bool ConfigHandler::RawNumber(
	[[maybe_unused]] const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	return false;
}

bool ConfigHandler::String(const Ch* str, SizeType length, bool copy)
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
	case State::PluginRequirements:
		return _pluginRequirements->String(str, length, copy);
	case State::Content:
		return _content->String(str, length, copy);
	case State::CustomContent:
		return _customContent->String(str, length, copy);
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
	case State::Pages:
		return _pages->String(str, length, copy);
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
	case State::Content:
		return _content->StartObject();
	case State::CustomContent:
		return _customContent->StartObject();
	case State::Pages:
		return _pages->StartObject();
	default:
		return false;
	}
}

bool ConfigHandler::Key(const Ch* str, SizeType length, bool copy)
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
			_state = State::PluginRequirements;
			_pluginRequirements = std::make_unique<PluginRequirementsHandler>();
			return true;
		}
		else if (strcmp(str, "content") == 0) {
			auto pageLayout = std::make_shared<PageLayout>();
			_config->MainPage = pageLayout;
			_content = std::make_unique<ContentHandler>(pageLayout.get(), _script);
			_state = State::Content;
			return true;
		}
		else if (strcmp(str, "customContent") == 0) {
			auto customContent = std::make_shared<CustomContent>();
			_config->MainPage = customContent;
			_customContent = std::make_unique<CustomContentHandler>(customContent.get());
			_state = State::CustomContent;
			return true;
		}
		else if (strcmp(str, "cursorFillMode") == 0) {
			_state = State::CursorFillMode;
			return true;
		}
		else if (strcmp(str, "pages") == 0) {
			_pages = std::make_unique<PagesHandler>(_config, _script);
			_state = State::Pages;
			return true;
		}
		else {
			return false;
		}
	case State::Content:
		return _content->Key(str, length, copy);
	case State::CustomContent:
		return _customContent->Key(str, length, copy);
	case State::Pages:
		return _pages->Key(str, length, copy);
	default:
		return false;
	}
}

bool ConfigHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Start:
		if (auto pageLayout = std::dynamic_pointer_cast<PageLayout>(_config->MainPage)) {
			pageLayout->CursorFillMode = _cursorFillMode;
		}
		_state = State::End;
		return true;
	case State::Content:
	{
		bool contentOK = _content->EndObject(memberCount);
		if (_content->Complete())
			_state = State::Start;
		return contentOK;
	}
	case State::CustomContent:
	{
		bool customContentOK = _customContent->EndObject(memberCount);
		if (_customContent->Complete())
			_state = State::Start;
		return customContentOK;
	}
	case State::Pages:
	{
		bool pagesOK = _pages->EndObject(memberCount);
		if (_pages->Complete())
			_state = State::Start;
		return pagesOK;
	}
	default:
		return false;
	}
}

bool ConfigHandler::StartArray()
{
	switch (_state) {
	case State::PluginRequirements:
		return _pluginRequirements->StartArray();
	case State::Content:
		return _content->StartArray();
	case State::Pages:
		return _pages->StartArray();
	default:
		return false;
	}
}

bool ConfigHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::PluginRequirements:
	{
		bool pluginRequirementsOK = _pluginRequirements->EndArray(elementCount);
		if (_pluginRequirements->Complete())
			_state = State::Start;
		return pluginRequirementsOK;
	}
	case State::Content:
	{
		bool contentOK = _content->EndArray(elementCount);
		if (_content->Complete())
			_state = State::Start;
		return contentOK;
	}
	case State::Pages:
	{
		bool pagesOK = _pages->EndArray(elementCount);
		if (_pages->Complete())
			_state = State::Start;
		return pagesOK;
	}
	default:
		return false;
	}
}
