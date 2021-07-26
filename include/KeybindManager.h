#pragma once

#include "Config/Action.h"

class KeybindInfo
{
public:
	std::string KeybindID;
	// TODO where is this used?
	std::string KeybindDesc;
	std::string ModName;
	std::shared_ptr<Action> Action;

	bool operator==(const KeybindInfo& other)
	{
		return KeybindID == other.KeybindID &&
			ModName == other.ModName &&
			Action == other.Action;
	}
};

class KeybindManager
{
public:
	static auto GetInstance() -> KeybindManager&;

	void ReadKeybinds(const std::string& a_modName);
	void ReadKeybindRegistrations();
	void CommitKeybinds();

	void Register(
		std::uint32_t a_keyCode,
		const std::string& a_modName,
		const std::string& a_ID);

	void AddKeybind(
		const std::string& a_modName,
		const std::string& a_keybindID,
		const KeybindInfo& a_info);

	auto GetKeybind(const std::string& a_modName, const std::string& a_keybindID) const
		-> KeybindInfo;

	auto GetRegisteredKey(const std::string& a_modName, const std::string& a_keybindID)
		-> std::uint32_t;

	void ClearKeybind(const std::string& a_modName, const std::string& a_keybindID);

	void ClearKeybind(std::uint32_t a_keyCode);

	void ProcessButtonEvent(RE::ButtonEvent* a_event) const;

private:
	bool _keybindsDirty;
	std::mutex _mutex;
	std::map<std::string, std::uint32_t> _modRegs;
	std::unordered_map<std::string, KeybindInfo> _modKeys;
	std::unordered_multimap<std::uint32_t, KeybindInfo> _lookup;
};