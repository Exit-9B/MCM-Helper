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
			std::vector<std::string> a_options,
			std::vector<std::string> a_shortNames);

		// Mod Settings

		static std::int32_t GetModSettingInt(RE::TESQuest* a_self, std::string_view a_settingName);

		static bool GetModSettingBool(RE::TESQuest* a_self, std::string_view a_settingName);

		static float GetModSettingFloat(RE::TESQuest* a_self, std::string_view a_settingName);

		static std::string GetModSettingString(
			RE::TESQuest* a_self,
			std::string_view a_settingName);

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

		static LatentResult<> OnPageReset(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::string a_page);

		static void OnOptionHighlight(RE::TESQuest* a_self, std::int32_t a_option);

		static LatentResult<> OnOptionSelect(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option);

		static LatentResult<> OnOptionDefault(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option);

		static void OnOptionSliderOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static LatentResult<> OnOptionSliderAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			float a_value);

		static void OnOptionMenuOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static LatentResult<> OnOptionMenuAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::int32_t a_index);

		static void OnOptionColorOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static LatentResult<> OnOptionColorAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::uint32_t a_color);

		static LatentResult<> OnOptionKeyMapChange(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::uint32_t a_keyCode,
			std::string_view a_conflictControl,
			std::string_view a_conflictName);

		static void OnOptionInputOpen(RE::TESQuest* a_self, std::int32_t a_option);

		static LatentResult<> OnOptionInputAccept(
			RE::BSScript::IVirtualMachine* a_vm,
			RE::VMStackID a_stackID,
			RE::TESQuest* a_self,
			std::int32_t a_option,
			std::string_view a_input);

		static std::string GetCustomControl(RE::TESQuest* a_self, std::uint32_t a_keyCode);

		// Papyrus Helper

		static void LoadConfig(RE::TESQuest* a_self);

		// Helpers

		static void UpdateInfoText(const ScriptObjectPtr& a_object, bool a_forceUpdate);

		[[nodiscard]] static VMAwaitable SendSettingChangeEvent(
			RE::BSScript::IVirtualMachine* a_vm,
			ScriptObjectPtr a_object,
			std::string a_ID);

		[[nodiscard]] static VMAwaitable SendPageSelectEvent(
			RE::BSScript::IVirtualMachine* a_vm,
			ScriptObjectPtr a_object,
			std::string a_page);

		// Registration delegate
		static bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm);
	};
}
