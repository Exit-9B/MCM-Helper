#pragma once

namespace Papyrus
{
	class MCM
	{
	public:
		inline static const char* ScriptName = "MCM";

		static bool IsInstalled(RE::StaticFunctionTag*);

		static std::uint32_t GetVersionCode(RE::StaticFunctionTag*);

		static std::int32_t GetModSettingInt(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName);

		static bool GetModSettingBool(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName);

		static float GetModSettingFloat(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName);

		static std::string_view GetModSettingString(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName);

		static void SetModSettingInt(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName,
			std::int32_t aiValue);

		static void SetModSettingBool(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName,
			bool abValue);

		static void SetModSettingFloat(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName,
			float afValue);

		static void SetModSettingString(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName,
			std::string_view asValue);

		static bool RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm);
	};
}
