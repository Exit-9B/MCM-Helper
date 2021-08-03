#include "Json/PagesHandler.h"
#include "Json/ContentHandler.h"
#include "Json/CustomContentHandler.h"

PagesHandler::PagesHandler(ReaderHandler* master, Config* config, const ScriptObjectPtr& script) :
	IHandler{ master },
	_config{ config },
	_script{ script }
{
}

bool PagesHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::PageDisplayName:
		_pageDisplayName = str;
		_state = State::Page;
		return true;
	case State::CursorFillMode:
		if (strcmp(str, "leftToRight") == 0) {
			_cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
			_state = State::Page;
			return true;
		}
		else if (strcmp(str, "topToBottom") == 0) {
			_cursorFillMode = SkyUI::CursorFillMode::TopToBottom;
			_state = State::Page;
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidValue, str);
		}
	default:
		return IHandler::String(str, length, copy);
	}
}

bool PagesHandler::StartObject()
{
	switch (_state) {
	case State::Main:
		_state = State::Page;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool PagesHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Page:
		if (strcmp(str, "pageDisplayName") == 0) {
			_state = State::PageDisplayName;
			return true;
		}
		else if (strcmp(str, "content") == 0) {
			auto pageLayout = std::make_shared<PageLayout>();
			_pageContent = pageLayout;
			_master->PushHandler<ContentHandler>(pageLayout.get(), _script);
			return true;
		}
		else if (strcmp(str, "customContent") == 0) {
			auto customContent = std::make_shared<CustomContent>();
			_pageContent = customContent;
			_master->PushHandler<CustomContentHandler>(customContent.get());
			return true;
		}
		else if (strcmp(str, "cursorFillMode") == 0) {
			_state = State::CursorFillMode;
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool PagesHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Page:
		if (!_pageDisplayName.empty() && _pageContent) {
			if (auto pageLayout = std::dynamic_pointer_cast<PageLayout>(_pageContent)) {
				pageLayout->CursorFillMode = _cursorFillMode;
			}
			_config->SubPages[_pageDisplayName] = _pageContent;
			_pageDisplayName.clear();
			_cursorFillMode = SkyUI::CursorFillMode::LeftToRight;
			_pageContent.reset();
		}
		_state = State::Main;
		return true;
	default:
		return IHandler::EndObject(memberCount);
	}
}

bool PagesHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartArray();
	}
}

bool PagesHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}