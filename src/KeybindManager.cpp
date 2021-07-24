#include "KeybindManager.h"

auto KeybindManager::GetInstance() -> KeybindManager&
{
	static KeybindManager instance;
	return instance;
}

void KeybindManager::ReadKeybinds(const std::string& a_modName)
{
	std::filesystem::path configPath{ "Data/MCM/Config/"sv };
	auto keybindsLocation = configPath / a_modName / "keybinds.json"sv;
	auto keybindsDirEntry = std::filesystem::directory_entry{ keybindsLocation };
	if (!keybindsDirEntry.exists())
		return;
}

void KeybindManager::Register(
	std::uint32_t a_keyCode,
	const std::string& a_modName,
	const std::string& a_keybindID)
{
	// Only one action can be assigned to a key
	for (auto it = _modRegs.begin(); it != _modRegs.end(); ++it)
	{
		if (it->second == a_keyCode)
		{
			_modRegs.erase(it);
			break;
		}
	}

	// Remove any previous binding then add the new one
	auto& reg = _modRegs[a_modName + ":"s + a_keybindID];
	_lookup.erase(reg);
	reg = a_keyCode;
	_lookup[a_keyCode] = KeybindInfo{
		.KeybindID = a_keybindID,
		.ModName = a_modName,
		.Action = _modKeys[a_modName + ":"s + a_keybindID],
	};
}

auto KeybindManager::GetKeybind(const std::string& a_modName, const std::string& a_keybindID)
	-> KeybindInfo
{
	return KeybindInfo{
		.KeybindID = a_keybindID,
		.ModName = a_modName,
		.Action = _modKeys[a_modName + ":"s + a_keybindID],
	};
}

auto KeybindManager::GetKeybind(std::uint32_t a_keyCode) -> KeybindInfo
{
	auto item = _lookup.find(a_keyCode);
	return item != _lookup.end() ? item->second : KeybindInfo{};
}

void KeybindManager::ClearKeybind(const std::string& a_modName, const std::string& a_keybindID)
{
	auto key = a_modName + ":"s + a_keybindID;
	auto item = _modRegs.find(key);
	if (item != _modRegs.end())
	{
		auto keyCode = item->second;
		_lookup.erase(keyCode);
		_modRegs.erase(item);
	}
}

void KeybindManager::ClearKeybind(std::uint32_t a_keyCode)
{
	auto& params = _lookup[a_keyCode];
	_modRegs.erase(params.ModName + ":"s + params.KeybindID);
}

void KeybindManager::ProcessButtonEvent(RE::ButtonEvent* a_event)
{
	assert(a_event);

	auto item = _lookup.find(a_event->GetIDCode());
	if (item != _lookup.end())
	{
		auto& action = item->second.Action;
		if (action)
		{
			action->SendControlEvent(a_event->IsUp(), a_event->HeldDuration());
		}
	}
}