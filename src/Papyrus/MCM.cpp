#include "Papyrus/MCM.h"
#include "SettingStore.h"

#define REGISTER_FUNCTION(vm, func) vm->RegisterFunction(#func##sv, ScriptName, func)
#define REGISTER_FUNCTION_ND(vm, func) vm->RegisterFunction(#func##sv, ScriptName, func, true)

bool Papyrus::MCM::IsInstalled(RE::StaticFunctionTag*)
{
	return true;
}

std::uint32_t Papyrus::MCM::GetVersionCode(RE::StaticFunctionTag*)
{
	return MCM_VERSION_RELEASE;
}

std::int32_t Papyrus::MCM::GetModSettingInt(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
{
	return SettingStore::GetInstance().GetModSettingInt(a_modName, a_settingName);
}

bool Papyrus::MCM::GetModSettingBool(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
{
	return SettingStore::GetInstance().GetModSettingBool(a_modName, a_settingName);
}

float Papyrus::MCM::GetModSettingFloat(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
{
	return SettingStore::GetInstance().GetModSettingFloat(a_modName, a_settingName);
}

std::string_view Papyrus::MCM::GetModSettingString(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName)
{
	auto s = SettingStore::GetInstance().GetModSettingString(a_modName, a_settingName);
	return s ? std::string_view{ s } : ""sv;
}

void Papyrus::MCM::SetModSettingInt(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	std::int32_t a_value)
{
	SettingStore::GetInstance().SetModSettingInt(a_modName, a_settingName, a_value);
}

void Papyrus::MCM::SetModSettingBool(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	bool a_value)
{
	SettingStore::GetInstance().SetModSettingBool(a_modName, a_settingName, a_value);
}

void Papyrus::MCM::SetModSettingFloat(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	float a_value)
{
	SettingStore::GetInstance().SetModSettingFloat(a_modName, a_settingName, a_value);
}

void Papyrus::MCM::SetModSettingString(
	RE::StaticFunctionTag*,
	std::string_view a_modName,
	std::string_view a_settingName,
	std::string_view a_value)
{
	SettingStore::GetInstance().SetModSettingString(a_modName, a_settingName, a_value);
}

bool Papyrus::MCM::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
{
	REGISTER_FUNCTION_ND(a_vm, IsInstalled);
	REGISTER_FUNCTION_ND(a_vm, GetVersionCode);

	REGISTER_FUNCTION_ND(a_vm, GetModSettingInt);
	REGISTER_FUNCTION_ND(a_vm, GetModSettingBool);
	REGISTER_FUNCTION_ND(a_vm, GetModSettingFloat);
	REGISTER_FUNCTION_ND(a_vm, GetModSettingString);

	REGISTER_FUNCTION_ND(a_vm, SetModSettingInt);
	REGISTER_FUNCTION_ND(a_vm, SetModSettingBool);
	REGISTER_FUNCTION_ND(a_vm, SetModSettingFloat);
	REGISTER_FUNCTION_ND(a_vm, SetModSettingString);

	return true;
}
