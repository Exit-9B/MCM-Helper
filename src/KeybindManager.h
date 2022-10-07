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

	bool operator==(const KeybindInfo& other) const { return Keybind == other.Keybind; }
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

class KeybindManager final
{
public:
	[[nodiscard]] static KeybindManager& GetInstance();

	void ReadKeybinds(const std::string& a_modName);
	void ReadKeybindRegistrations();
	void CommitKeybinds();

	void Register(std::uint32_t a_keyCode, const std::string& a_modName, const std::string& a_ID);

	void AddKeybind(
		const std::string& a_modName,
		const std::string& a_keybindID,
		const KeybindInfo& a_info);

	[[nodiscard]] KeybindInfo GetKeybind(
		const std::string& a_modName,
		const std::string& a_keybindID) const;

	[[nodiscard]] std::uint32_t GetRegisteredKey(
		const std::string& a_modName,
		const std::string& a_keybindID);

	void Unregister(const std::string& a_modName, const std::string& a_keybindID);

	void Unregister(std::uint32_t a_keyCode);

	void ClearModKeys();

	void ProcessButtonEvent(const RE::ButtonEvent* a_event) const;

private:
	bool _keybindsDirty;
	std::mutex _mutex;
	std::map<Keybind, std::uint32_t> _modRegs;
	std::unordered_map<Keybind, KeybindInfo> _modKeys;
	std::unordered_multimap<std::uint32_t, KeybindInfo> _lookup;
};
