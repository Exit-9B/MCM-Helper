#include "KeybindManager.h"
#include "Json/ReaderHandler.h"
#include "Json/KeybindsHandler.h"
#include "Json/UserKeybindsHandler.h"
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

auto KeybindManager::GetInstance() -> KeybindManager&
{
	static KeybindManager instance;
	return instance;
}

void KeybindManager::ReadKeybinds(const std::string& a_modName)
{
	std::filesystem::path configPath{ "Data/MCM/Config"sv };
	auto keybindsLocation = configPath / a_modName / "keybinds.json"sv;

	auto keybindsDirEntry = std::filesystem::directory_entry{ keybindsLocation };
	if (!keybindsDirEntry.exists())
		return;

	ReaderHandler handler;
	handler.PushHandler<KeybindsHandler>(std::addressof(handler), a_modName);

	FILE* fp = nullptr;
	auto err = _wfopen_s(std::addressof(fp), keybindsLocation.c_str(), L"r");
	if (err != 0)
	{
		logger::warn("Failed to open keybinds for {}"sv, a_modName);
		return;
	}

	char readBuffer[65536]{};
	rapidjson::FileReadStream is{ fp, readBuffer, sizeof(readBuffer) };
	rapidjson::Reader reader;

	auto result = reader.Parse(is, handler);
	fclose(fp);
	if (!result)
	{
		logger::warn("Failed to parse keybinds for {}"sv, a_modName);
	}
}

void KeybindManager::ReadKeybindRegistrations()
{
	auto startTime = std::chrono::steady_clock::now();

	auto settingsPath = std::filesystem::path{ "Data/MCM/Settings"sv };
	auto keybindsLocation = settingsPath / "keybinds.json"sv;

	ReaderHandler handler;
	handler.PushHandler<UserKeybindsHandler>(std::addressof(handler));

	FILE* fp = nullptr;
	auto err = _wfopen_s(std::addressof(fp), keybindsLocation.c_str(), L"r");
	if (err != 0)
	{
		return;
	}

	char readBuffer[65536]{};
	rapidjson::FileReadStream is{ fp, readBuffer, sizeof(readBuffer) };
	rapidjson::Reader reader;

	auto result = reader.Parse(is, handler);
	fclose(fp);
	if (!result)
	{
		logger::warn("Failed to parse keybind registrations"sv);
	}

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

	logger::info(
		"Loaded keybind registrations in {} us."sv,
		elapsedMs.count());
}

void KeybindManager::CommitKeybinds()
{
	if (!_keybindsDirty)
		return;

	auto startTime = std::chrono::steady_clock::now();

	{
		std::scoped_lock lock{ _mutex };

		auto settingsPath = std::filesystem::path{ "Data/MCM/Settings"sv };
		auto settingsEntry = std::filesystem::directory_entry{ settingsPath };
		if (!settingsEntry.exists())
		{
			std::filesystem::create_directory(settingsPath);
		}

		auto keybindsLocation = settingsPath / "keybinds.json"sv;
		FILE* fp = nullptr;
		auto err = _wfopen_s(std::addressof(fp), keybindsLocation.c_str(), L"w");
		if (err != 0)
		{
			logger::error("Failed to open file for saving keybind registrations"sv);
			return;
		}

		char writeBuffer[65536]{};
		rapidjson::FileWriteStream os{ fp, writeBuffer, sizeof(writeBuffer) };
		rapidjson::Writer<rapidjson::FileWriteStream> writer{ os };

		writer.StartObject();
		writer.Key("version");
		writer.String(std::to_string(PLUGIN_VERSION).c_str());
		writer.Key("keybinds");
		writer.StartArray();

		for (auto& [modKeybindID, keyCode] : _modRegs)
		{
			// TODO: Implement some better data structure so we're not doing this
			std::string modName;
			std::string keybindID;
			std::istringstream ss{ modKeybindID };
			std::getline(ss, modName, ':');
			std::getline(ss, keybindID);

			writer.StartObject();
			writer.Key("keycode");
			writer.Uint(keyCode);
			writer.Key("modName");
			writer.String(modName.c_str());
			writer.Key("id");
			writer.String(keybindID.c_str());
			writer.EndObject();
		}

		writer.EndArray();
		writer.EndObject();

		_keybindsDirty = false;
	}

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

	logger::info(
		"Saved keybind registrations in {} us."sv,
		elapsedMs.count());
}

void KeybindManager::Register(
	std::uint32_t a_keyCode,
	const std::string& a_modName,
	const std::string& a_keybindID)
{
	if (GetRegisteredKey(a_modName, a_keybindID) == a_keyCode)
		return;

	auto key = a_modName + ":"s + a_keybindID;

	std::scoped_lock lock{ _mutex };

	auto regIt = _modRegs.find(key);
	auto keyIt = _modKeys.find(key);

	// The keybind might not have been added yet
	if (keyIt != _modKeys.end())
	{
		// A keybind can only have one key
		if (regIt != _modRegs.end())
		{
			for (auto [it, end] = _lookup.equal_range(regIt->second); it != end; ++it)
			{
				if (it->second == keyIt->second)
				{
					_lookup.erase(it);
					break;
				}
			}
		}

		_lookup.emplace(std::make_pair(a_keyCode, keyIt->second));
	}

	_modRegs[key] = a_keyCode;
	_keybindsDirty = true;
}

void KeybindManager::AddKeybind(
	const std::string& a_modName,
	const std::string& a_keybindID,
	const KeybindInfo& a_info)
{
	auto key = a_modName + ":"s + a_keybindID;
	_modKeys[key] = a_info;

	// Add to lookup if a key was already registered
	auto regIt = _modRegs.find(key);
	if (regIt != _modRegs.end())
	{
		_lookup.emplace(regIt->second, a_info);
	}
}

auto KeybindManager::GetKeybind(const std::string& a_modName, const std::string& a_keybindID) const
	-> KeybindInfo
{
	auto key = a_modName + ":"s + a_keybindID;
	auto item = _modKeys.find(key);
	return item != _modKeys.end() ? item->second : KeybindInfo{};
}

auto KeybindManager::GetKeybind(std::uint32_t a_keyCode) const -> KeybindInfo
{
	auto item = _lookup.find(a_keyCode);
	return item != _lookup.end() ? item->second : KeybindInfo{};
}

auto KeybindManager::GetRegisteredKey(
	const std::string& a_modName,
	const std::string& a_keybindID)
	-> std::uint32_t
{
	std::scoped_lock lock{ _mutex };

	auto key = a_modName + ":"s + a_keybindID;
	auto item = _modRegs.find(key);
	return item != _modRegs.end() ? item->second : -1;
}

void KeybindManager::ClearKeybind(const std::string& a_modName, const std::string& a_keybindID)
{
	std::scoped_lock lock{ _mutex };

	auto key = a_modName + ":"s + a_keybindID;
	auto item = _modRegs.find(key);
	if (item != _modRegs.end())
	{
		auto keyCode = item->second;
		_lookup.erase(keyCode);
		_modRegs.erase(item);

		_keybindsDirty = true;
	}
}

void KeybindManager::ClearKeybind(std::uint32_t a_keyCode)
{
	std::scoped_lock lock{ _mutex };

	auto item = _lookup.find(a_keyCode);
	if (item != _lookup.end())
	{
		auto& params = item->second;
		_modRegs.erase(params.ModName + ":"s + params.KeybindID);

		_keybindsDirty = true;
	}
}

void KeybindManager::ProcessButtonEvent(RE::ButtonEvent* a_event) const
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