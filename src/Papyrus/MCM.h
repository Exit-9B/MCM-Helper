#pragma once

namespace Papyrus
{
	class MCM
	{
	public:
		inline static const char* ScriptName = "MCM";

		static auto IsInstalled(RE::StaticFunctionTag*) -> bool;

		static auto GetVersionCode(RE::StaticFunctionTag*) -> std::uint32_t;

		static auto GetModSettingInt(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName)
			-> std::int32_t;

		static auto GetModSettingBool(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName)
			-> bool;

		static auto GetModSettingFloat(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName)
			-> float;

		static auto GetModSettingString(RE::StaticFunctionTag*,
			std::string_view asModName,
			std::string_view asSettingName)
			-> std::string_view;

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