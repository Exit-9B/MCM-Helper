#pragma once

#include "Config/Action.h"

class KeybindInfo
{
public:
	std::string KeybindID;
	std::string KeybindDesc;
	std::string ModName;
	std::shared_ptr<Action> Action;
};

class KeybindManager
{
public:
	static auto GetInstance() -> KeybindManager&;

	void ReadKeybinds(const std::string& a_modName);

	void Register(
		std::uint32_t a_keyCode,
		const std::string& a_modName,
		const std::string& a_ID);

	void AddKeybind(
		const std::string& a_modName,
		const std::string& a_keybindID,
		const KeybindInfo& a_info);

	auto GetKeybind(const std::string& a_modName, const std::string& a_keybindID) -> KeybindInfo;
	auto GetKeybind(std::uint32_t a_keyCode) -> KeybindInfo;

	auto GetRegisteredKey(const std::string& a_modName, const std::string& a_keybindID)
		-> std::int32_t;

	void ClearKeybind(const std::string& a_modName, const std::string& a_keybindID);

	void ClearKeybind(std::uint32_t a_keyCode);

	void ProcessButtonEvent(RE::ButtonEvent* a_event);

private:
	std::unordered_map<std::string, KeybindInfo> _modKeys;
	std::unordered_map<std::string, std::uint32_t> _modRegs;
	std::unordered_map<std::uint32_t, KeybindInfo> _lookup;
};