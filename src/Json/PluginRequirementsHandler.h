#pragma once

#include "Json/IHandler.h"

class PluginRequirementsHandler final : public IHandler
{
public:
	PluginRequirementsHandler(ReaderHandler* master);

	virtual bool String(const Ch* str, SizeType length, bool copy) override;
	virtual bool StartArray() override;
	virtual bool EndArray(SizeType elementCount) override;

private:
	enum class State
	{
		Init,
		Main,
	};

	State _state = State::Init;
};
