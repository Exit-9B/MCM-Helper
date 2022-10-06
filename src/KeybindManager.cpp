#include "KeybindManager.h"
#include "Json/ReaderHandler.h"
#include "Json/KeybindsHandler.h"
#include "Json/UserKeybindsHandler.h"
#include "InputMap.h"
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>

KeybindManager& KeybindManager::GetInstance()
{
	static KeybindManager instance{};
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
	handler.PushHandler<KeybindsHandler>(a_modName);

	if (!handler.ReadFile(keybindsLocation)) {
		logger::warn("Failed to parse keybinds for {}"sv, a_modName);
	}
}

void KeybindManager::ReadKeybindRegistrations()
{
	auto startTime = std::chrono::steady_clock::now();

	auto settingsPath = std::filesystem::path{ "Data/MCM/Settings"sv };
	auto keybindsLocation = settingsPath / "keybinds.json"sv;

	ReaderHandler handler;
	handler.PushHandler<UserKeybindsHandler>();

	if (!handler.ReadFile(keybindsLocation)) {
		logger::warn("Failed to parse keybind registrations"sv);
	}

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	logger::info("Loaded keybind registrations in {} ms."sv, elapsedMs.count());
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
		if (!settingsEntry.exists()) {
			std::filesystem::create_directory(settingsPath);
		}

		auto keybindsLocation = settingsPath / "keybinds.json"sv;
		FILE* fp = nullptr;
		auto err = _wfopen_s(std::addressof(fp), keybindsLocation.c_str(), L"w");
		if (err != 0) {
			logger::error("Failed to open file for saving keybind registrations"sv);
			return;
		}

		char writeBuffer[65536]{};
		rapidjson::FileWriteStream os{ fp, writeBuffer, sizeof(writeBuffer) };
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer{ os };
		writer.SetIndent(' ', 2);

		writer.StartObject();
		writer.Key("version");
		writer.Uint(MCM_VERSION_RELEASE);
		writer.Key("keybinds");
		writer.StartArray();

		for (auto& [keybind, keyCode] : _modRegs) {
			writer.StartObject();
			writer.Key("keycode");
			writer.Int(static_cast<std::int32_t>(keyCode));
			writer.Key("modName");
			writer.String(keybind.ModName.c_str());
			writer.Key("id");
			writer.String(keybind.KeybindID.c_str());
			writer.EndObject();
		}

		writer.EndArray();
		writer.EndObject();
		fclose(fp);

		_keybindsDirty = false;
	}

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	logger::info("Saved keybind registrations in {} ms."sv, elapsedMs.count());
}

void KeybindManager::Register(
	std::uint32_t a_keyCode,
	const std::string& a_modName,
	const std::string& a_keybindID)
{
	if (GetRegisteredKey(a_modName, a_keybindID) == a_keyCode)
		return;

	Unregister(a_modName, a_keybindID);

	auto key = Keybind{ a_modName, a_keybindID };

	std::scoped_lock lock{ _mutex };

	// Check if keybind has been added yet
	auto keyIt = _modKeys.find(key);
	if (keyIt != _modKeys.end()) {
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
	auto key = Keybind{ a_modName, a_keybindID };
	_modKeys[key] = a_info;

	// Add to lookup if a key was already registered
	auto regIt = _modRegs.find(key);
	if (regIt != _modRegs.end()) {
		auto& keyCode = regIt->second;
		auto [it, end] = _lookup.equal_range(keyCode);

		// Try to prevent duplicates
		auto pred = [a_info](const std::pair<std::uint32_t, KeybindInfo>& entry)
		{
			return a_info.Keybind == entry.second.Keybind;
		};

		if (std::find_if(it, end, pred) == end) {
			_lookup.emplace(keyCode, a_info);
		}
	}
}

KeybindInfo KeybindManager::GetKeybind(const std::string& a_modName, const std::string& a_keybindID) const
{
	auto key = Keybind{ a_modName, a_keybindID };
	auto item = _modKeys.find(key);
	return item != _modKeys.end() ? item->second : KeybindInfo{};
}

std::uint32_t KeybindManager::GetRegisteredKey(const std::string& a_modName, const std::string& a_keybindID)
{
	std::scoped_lock lock{ _mutex };

	auto key = Keybind{ a_modName, a_keybindID };
	auto item = _modRegs.find(key);
	return item != _modRegs.end() ? item->second : 0;
}

void KeybindManager::Unregister(const std::string& a_modName, const std::string& a_keybindID)
{
	std::scoped_lock lock{ _mutex };

	auto key = Keybind{ a_modName, a_keybindID };
	auto regIt = _modRegs.find(key);
	auto keyIt = _modKeys.find(key);

	if (regIt != _modRegs.end()) {
		auto& keyCode = regIt->second;
		if (keyIt != _modKeys.end()) {
			auto& keyInfo = keyIt->second;

			for (auto [it, end] = _lookup.equal_range(keyCode); it != end; ++it) {
				if (it->second == keyInfo) {
					_lookup.erase(it);
					break;
				}
			}
		}

		_modRegs.erase(regIt);
		_keybindsDirty = true;
	}
}

void KeybindManager::Unregister(std::uint32_t a_keyCode)
{
	std::scoped_lock lock{ _mutex };

	_lookup.erase(a_keyCode);

	const auto count = std::erase_if(
		_modRegs,
		[=](const auto& item)
		{
			return item.second == a_keyCode;
		});

	if (count > 0) {
		_keybindsDirty = true;
	}
}

void KeybindManager::ClearModKeys()
{
	std::scoped_lock lock{ _mutex };

	_modKeys.clear();
	_lookup.clear();
}

void KeybindManager::ProcessButtonEvent(RE::ButtonEvent* a_event) const
{
	assert(a_event);

	std::uint32_t keyCode = a_event->GetIDCode();

	switch (a_event->device.get()) {
	case RE::INPUT_DEVICES::kMouse:
		keyCode = InputMap::kMacro_MouseButtonOffset + keyCode;
		break;
	case RE::INPUT_DEVICES::kGamepad:
		keyCode = InputMap::GamepadMaskToKeycode(keyCode);
		break;
	default:
		break;
	}

	for (auto [it, end] = _lookup.equal_range(keyCode); it != end; ++it) {
		auto& action = it->second.Action;
		if (action) {
			if (a_event->IsDown()) {
				action->SendControlEvent(false);
			}
			else if (a_event->IsUp()) {
				action->SendControlEvent(true, a_event->HeldDuration());
			}
		}
	}
}
