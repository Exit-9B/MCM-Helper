#pragma once

#include "Config/Action.h"

struct Keybind
{
	std::string ModName;
	std::string KeybindID;

	auto operator<=>(const Keybind& other) const = default;
};

struct KeybindInfo
{
	Keybind Keybind;
	std::string KeybindDesc;
	std::shared_ptr<Action> Action;
};

template <>
struct std::hash<Keybind>
{
	std::size_t operator()(const Keybind& keybind) const noexcept
	{
		return std::hash<std::string>{}(keybind.ModName) ^
			std::hash<std::string>{}(keybind.KeybindID);
	}
};

class KeybindManager
{
public:
	static auto GetInstance() -> KeybindManager&;

	void ReadKeybinds(const std::string& a_modName);
	void ReadKeybindRegistrations();
	void CommitKeybinds();

	void Register(std::uint32_t a_keyCode, const std::string& a_modName, const std::string& a_ID);

	void AddKeybind(
		const std::string& a_modName,
		const std::string& a_keybindID,
		const KeybindInfo& a_info);

	auto GetKeybind(const std::string& a_modName, const std::string& a_keybindID) const
		-> KeybindInfo;

	auto GetRegisteredKey(const std::string& a_modName, const std::string& a_keybindID)
		-> std::uint32_t;

	void Unregister(const std::string& a_modName, const std::string& a_keybindID);

	void Unregister(std::uint32_t a_keyCode);

	void ClearKeybinds();

	void ProcessButtonEvent(RE::ButtonEvent* a_event) const;

private:
	bool _keybindsDirty;
	std::mutex _mutex;
	std::map<Keybind, std::uint32_t> _modRegs;
	std::unordered_map<Keybind, KeybindInfo> _modKeys;
	std::unordered_multimap<std::uint32_t, KeybindInfo> _lookup;
};