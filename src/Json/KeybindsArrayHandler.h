#pragma once

#include "Json/IHandler.h"
#include "KeybindManager.h"

class KeybindsArrayHandler final : public IHandler
{
public:
	KeybindsArrayHandler(
		ReaderHandler* master,
		std::unordered_map<std::string, KeybindInfo>* modKeys);

	bool String(const Ch* str, SizeType length, bool copy) override;
	bool StartObject() override;
	bool Key(const Ch* str, SizeType length, bool copy) override;
	bool EndObject(SizeType memberCount) override;
	bool StartArray() override;
	bool EndArray(SizeType elementCount) override;

private:
	enum class State
	{
		Init,
		Main,
		Keybind,
		ID,
		Desc,
	};

	State _state = State::Init;

	std::string _id;
	std::string _desc;
	std::shared_ptr<Action> _action;
	std::unordered_map<std::string, KeybindInfo>* _modKeys;
};