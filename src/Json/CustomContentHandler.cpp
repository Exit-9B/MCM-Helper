#include "Json/CustomContentHandler.h"

CustomContentHandler::CustomContentHandler(ReaderHandler* master, CustomContent* customContent) :
	IHandler{ master },
	_customContent{ customContent }
{
}

bool CustomContentHandler::Int(int i)
{
	switch (_state) {
	case State::X:
		_customContent->X = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::Y:
		_customContent->Y = static_cast<float>(i);
		_state = State::Main;
		return true;
	default:
		return IHandler::Int(i);
	}
}

bool CustomContentHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::X:
		_customContent->X = static_cast<float>(i);
		_state = State::Main;
		return true;
	case State::Y:
		_customContent->Y = static_cast<float>(i);
		_state = State::Main;
		return true;
	default:
		return IHandler::Uint(i);
	}
}

bool CustomContentHandler::Double(double d)
{
	switch (_state) {
	case State::X:
		_customContent->X = static_cast<float>(d);
		_state = State::Main;
		return true;
	case State::Y:
		_customContent->Y = static_cast<float>(d);
		_state = State::Main;
		return true;
	default:
		return IHandler::Double(d);
	}
}

bool CustomContentHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Source:
		_customContent->Source = str;
		_state = State::Main;
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool CustomContentHandler::StartObject()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartObject();
	}
}

bool CustomContentHandler::Key(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Main:
		if (strcmp(str, "source") == 0) {
			_state = State::Source;
			return true;
		}
		else if (strcmp(str, "x") == 0) {
			_state = State::X;
			return true;
		}
		else if (strcmp(str, "y") == 0) {
			_state = State::Y;
			return true;
		}
		else {
			return ReportError(ErrorType::InvalidKey, str);
		}
	default:
		return IHandler::Key(str, length, copy);
	}
}

bool CustomContentHandler::EndObject(SizeType memberCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndObject(memberCount);
	}
}