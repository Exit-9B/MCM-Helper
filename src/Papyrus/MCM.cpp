#include "Papyrus/MCM.h"
#include "SettingStore.h"

#define REGISTER_FUNCTION(vm, func) vm->RegisterFunction(#func ## sv, ScriptName, func)

auto Papyrus::MCM::IsInstalled(RE::StaticFunctionTag*) -> bool
{
	return true;
}

auto Papyrus::MCM::GetVersionCode(RE::StaticFunctionTag*) -> std::uint32_t
{
	return PLUGIN_VERSION;
}

auto Papyrus::MCM::GetModSettingInt(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
	-> std::int32_t
{
	return SettingStore::GetInstance().GetModSettingInt(a_modName, a_settingName);
}

auto Papyrus::MCM::GetModSettingBool(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
	-> bool
{
	return SettingStore::GetInstance().GetModSettingBool(a_modName, a_settingName);
}

auto Papyrus::MCM::GetModSettingFloat(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
	-> float
{
	return SettingStore::GetInstance().GetModSettingFloat(a_modName, a_settingName);
}

auto Papyrus::MCM::GetModSettingString(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
	-> std::string_view
{
	auto s = SettingStore::GetInstance().GetModSettingString(a_modName, a_settingName);
	return s ? std::string_view{ s } : ""s;
}

void Papyrus::MCM::SetModSettingInt(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	std::int32_t a_value)
{
	SettingStore::GetInstance().SetModSettingInt(a_modName, a_settingName, a_value);
}

void Papyrus::MCM::SetModSettingBool(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	bool a_value)
{
	SettingStore::GetInstance().SetModSettingBool(a_modName, a_settingName, a_value);
}

void Papyrus::MCM::SetModSettingFloat(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	float a_value)
{
	SettingStore::GetInstance().SetModSettingFloat(a_modName, a_settingName, a_value);
}

void Papyrus::MCM::SetModSettingString(RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	std::string_view a_value)
{
	SettingStore::GetInstance().SetModSettingString(a_modName, a_settingName, a_value);
}

bool Papyrus::MCM::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
{
	REGISTER_FUNCTION(a_vm, IsInstalled);
	REGISTER_FUNCTION(a_vm, GetVersionCode);

	REGISTER_FUNCTION(a_vm, GetModSettingInt);
	REGISTER_FUNCTION(a_vm, GetModSettingBool);
	REGISTER_FUNCTION(a_vm, GetModSettingFloat);
	REGISTER_FUNCTION(a_vm, GetModSettingString);

	REGISTER_FUNCTION(a_vm, SetModSettingInt);
	REGISTER_FUNCTION(a_vm, SetModSettingBool);
	REGISTER_FUNCTION(a_vm, SetModSettingFloat);
	REGISTER_FUNCTION(a_vm, SetModSettingString);

	return true;
}