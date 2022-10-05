#pragma once

#include "Json/IHandler.h"
#include "KeybindManager.h"

class KeybindsHandler final : public IHandler
{
public:
	KeybindsHandler(ReaderHandler* master, const std::string& modName);

	bool String(const Ch* str, SizeType length, bool copy) override;
	bool StartObject() override;
	bool Key(const Ch* str, SizeType length, bool copy) override;
	bool EndObject(SizeType memberCount) override;

private:
	enum class State
	{
		Init,
		Main,
		Schema,
		ModName,
	};

	State _state = State::Init;

	std::string _modName;
	std::unordered_map<std::string, KeybindInfo> _modKeys;

	bool _hasModName = false;
};
