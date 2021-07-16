#pragma once

#include <rapidjson/reader.h>
#include "PageContent.h"

class CustomContentHandler
{
public:
	using Ch = rapidjson::UTF8<>::Ch;
	using SizeType = rapidjson::SizeType;

	CustomContentHandler(CustomContent* customContent);

	bool Complete();

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
	CustomContent* _customContent;
};
