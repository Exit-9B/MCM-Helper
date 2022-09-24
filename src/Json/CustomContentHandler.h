#pragma once

#include "Json/IHandler.h"
#include "Config/PageContent.h"

class CustomContentHandler final : public IHandler
{
public:
	CustomContentHandler(ReaderHandler* master, CustomContent* customContent);

	bool Int(int i) override;
	bool Uint(unsigned i) override;
	bool Double(double d) override;
	bool String(const Ch* str, SizeType length, bool copy) override;
	bool StartObject() override;
	bool Key(const Ch* str, SizeType length, bool copy) override;
	bool EndObject(SizeType memberCount) override;

private:
	enum class State
	{
		Init,
		Main,
		Source,
		X,
		Y,
	};

	State _state = State::Init;

	CustomContent* _customContent;
};
