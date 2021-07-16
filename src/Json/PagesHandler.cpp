#include "Json/PagesHandler.h"

PagesHandler::PagesHandler(Config* config, const ScriptObjectPtr& script) :
	_config{ config },
	_script{ script }
{
}

bool PagesHandler::Complete()
{
	return _state == State::End;
}

bool PagesHandler::Bool(bool b)
{
	switch (_state) {
	case State::Content:
		return _content->Bool(b);
	default:
		return false;
	}
}

bool PagesHandler::Int(int i)
{
	switch (_state) {
	case State::Content:
		return _content->Int(i);
	default:
		return false;
	}
}

bool PagesHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Content:
		return _content->Uint(i);
	default:
		return false;
	}
}

bool PagesHandler::Double(double d)
{
	switch (_state) {
	case State::Content:
		return _content->Double(d);
	default:
		return false;
	}
}

bool PagesHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::PageDisplayName:
		_pageDisplayName = str;
		_state = State::Page;
		return true;
	case State::Content:
		return _content->String(str, length, copy);
	case State::CustomContent:
		return _customContent->String(str, length, copy);
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
			return false;
		}
	default:
		return false;
	}
}

bool PagesHandler::StartObject()
{
	switch (_state) {
	case State::Start:
		_state = State::Page;
		return true;
	case State::Content:
		return _content->StartObject();
	case State::CustomContent:
		return _customContent->StartObject();
	default:
		return false;
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
			_content = std::make_unique<ContentHandler>(pageLayout.get(), _script);
			_state = State::Content;
			return true;
		}
		else if (strcmp(str, "customContent") == 0) {
			auto customContent = std::make_shared<CustomContent>();
			_pageContent = customContent;
			_customContent = std::make_unique<CustomContentHandler>(customContent.get());
			_state = State::CustomContent;
			return true;
		}
		else if (strcmp(str, "cursorFillMode") == 0) {
			_state = State::CursorFillMode;
			return true;
		}
		else {
			return false;
		}
	case State::Content:
		return _content->Key(str, length, copy);
	case State::CustomContent:
		return _customContent->Key(str, length, copy);
	default:
		return false;
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
		_state = State::Start;
		return true;
	case State::Content:
		return _content->EndObject(memberCount);
	case State::CustomContent:
	{
		bool customContentOK = _customContent->EndObject(memberCount);
		if (_customContent->Complete())
			_state = State::Start;
		return customContentOK;
	}
	default:
		return false;
	}
}

bool PagesHandler::StartArray()
{
	switch (_state) {
	case State::End:
		_state = State::Start;
		return true;
	case State::Content:
		return _content->StartArray();
	default:
		return false;
	}
}

bool PagesHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Start:
		_state = State::End;
		return true;
	case State::Content:
	{
		bool contentOK = _content->EndArray(elementCount);
		if (_content->Complete())
			_state = State::Page;
		return contentOK;
	}
	default:
		return false;
	}
}