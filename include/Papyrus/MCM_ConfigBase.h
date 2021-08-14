#pragma once

namespace Papyrus
{
	class MCM_ConfigBase
	{
	public:
		inline static const char* ScriptName = "MCM_ConfigBase";

		// MCM
		static void RefreshMenu(RE::TESQuest* a_self);

		static void SetMenuOptions(
			RE::TESQuest* a_self,
			std::string a_ID,
			std::vector<RE::BSFixedString> a_options,
			std::vector<RE::BSFixedString> a_shortNames);

		// Mod Settings
		static auto GetModSettingInt(
			RE::TESQuest* a_self,
			std::string_view a_settingName)
			-> std::int32_t;

		static auto GetModSettingBool(
			RE::TESQuest* a_self,
			std::string_view a_settingName)
			-> bool;

		static auto GetModSettingFloat(
			RE::TESQuest* a_self,
			std::string_view a_settingName)
			-> float;

		static auto GetModSettingString(
			RE::TESQuest* a_self,
			std::string_view a_settingName)
			-> std::string;

		static void SetModSettingInt(
			RE::TESQuest* a_self,
			std::string_view a_settingName,
			std::int32_t a_value);

		static void SetModSettingBool(
			RE::TESQuest* a_self,
			std::string_view a_settingName,
			bool a_value);

		static void SetModSettingFloat(
			RE::TESQuest* a_self,
			std::string_view a_settingName,
			float a_value);

		static void SetModSettingString(
			RE::TESQuest* a_self,
			std::string_view a_settingName,
			std::string_view a_value);

		// SkyUI Overrides
		static void OnPageReset(RE::TESQuest* a_self, std::string a_page);

		static void OnOptionHighlight(RE::TESQuest* a_self, std::int32_t a_option);

		static void OnOptionSelect(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option);

		static void OnOptionDefault(RE::TESQuest* a_self, std::int32_t a_option);

		static void OnOptionSliderOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static void OnOptionSliderAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			float a_value);

		static void OnOptionMenuOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static void OnOptionMenuAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::int32_t a_index);

		static void OnOptionColorOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static void OnOptionColorAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::uint32_t a_color);

		static void OnOptionKeyMapChange(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::uint32_t a_keyCode,
			std::string_view a_conflictControl,
			std::string_view a_conflictName);

		static void OnOptionInputOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static void OnOptionInputAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::string_view a_input);

		static auto GetCustomControl(RE::TESQuest* a_self, std::uint32_t a_keyCode) -> std::string;

		// Helpers
		static void LoadConfig(RE::TESQuest* a_self);

		static void UpdateInfoText(ScriptObjectPtr a_object, bool a_forceUpdate);

		static void SendSettingChangeEvent(
			RE::BSScript::IVirtualMachine* a_vm,
			ScriptObjectPtr a_object,
			std::string a_ID);

		// Registration delegate
		static bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm);
	};
}
