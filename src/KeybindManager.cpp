#include "KeybindManager.h"
#include "Json/ReaderHandler.h"
#include "Json/KeybindsHandler.h"
#include <rapidjson/filereadstream.h>

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

	ReaderHandler handler;
	handler.PushHandler<KeybindsHandler>(std::addressof(handler), a_modName);

	rapidjson::Reader reader;

	FILE* fp = nullptr;
	auto err = _wfopen_s(std::addressof(fp), keybindsLocation.c_str(), L"r");
	if (err != 0)
	{
		logger::warn("Failed to open keybinds for {}"sv, a_modName);
		return;
	}

	char readBuffer[65536]{};
	rapidjson::FileReadStream is{ fp, readBuffer, sizeof(readBuffer) };

	auto result = reader.Parse(is, handler);
	fclose(fp);
	if (!result)
	{
		logger::warn("Failed to parse keybinds for {}"sv, a_modName);
	}
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
	auto key = a_modName + ":"s + a_keybindID;
	auto& reg = _modRegs[key];
	_lookup.erase(reg);
	reg = a_keyCode;
	_lookup[a_keyCode] = _modKeys[key];
}

void KeybindManager::AddKeybind(
	const std::string& a_modName,
	const std::string& a_keybindID,
	const KeybindInfo& a_info)
{
	auto key = a_modName + ":"s + a_keybindID;
	_modKeys[key] = a_info;
}

auto KeybindManager::GetKeybind(const std::string& a_modName, const std::string& a_keybindID)
	-> KeybindInfo
{
	auto key = a_modName + ":"s + a_keybindID;
	auto item = _modKeys.find(key);
	return item != _modKeys.end() ? item->second : KeybindInfo{};
}

auto KeybindManager::GetKeybind(std::uint32_t a_keyCode) -> KeybindInfo
{
	auto item = _lookup.find(a_keyCode);
	return item != _lookup.end() ? item->second : KeybindInfo{};
}

auto KeybindManager::GetRegisteredKey(const std::string& a_modName, const std::string& a_keybindID)
	-> std::int32_t
{
	auto key = a_modName + ":"s + a_keybindID;
	auto item = _modRegs.find(key);
	return item != _modRegs.end() ? item->second : -1;
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
	auto item = _lookup.find(a_keyCode);
	if (item != _lookup.end())
	{
		auto& params = item->second;
		_modRegs.erase(params.ModName + ":"s + params.KeybindID);
	}
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