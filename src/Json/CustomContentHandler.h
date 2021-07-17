#pragma once

#include "Json/ReaderHandler.h"
#include "PageContent.h"

class CustomContentHandler : public IHandler
{
public:
	CustomContentHandler(ReaderHandler* master, CustomContent* customContent);

	bool Int(int i);
	bool Uint(unsigned i);
	bool Double(double d);
	bool String(const Ch* str, SizeType length, bool copy);
	bool StartObject();
	bool Key(const Ch* str, SizeType length, bool copy);
	bool EndObject(SizeType memberCount);

private:
	enum class State
	{
		End,
		Start,
		Source,
		X,
		Y,
	};

	State _state = State::End;

	ReaderHandler* _master;
	CustomContent* _customContent;
};
