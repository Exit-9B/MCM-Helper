#include "ScriptObject.h"
#include "Utils.h"

auto ScriptObject::GetBool(ScriptObjectPtr a_object, std::string_view a_variableName) -> bool
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	return variable ? variable->GetBool() : false;
}

void ScriptObject::SetBool(ScriptObjectPtr a_object, std::string_view a_variableName, bool a_value)
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetBool(a_value);
}

auto ScriptObject::GetInt(ScriptObjectPtr a_object, std::string_view a_variableName) -> std::int32_t
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	return variable ? variable->GetSInt() : 0;
}

void ScriptObject::SetInt(
	ScriptObjectPtr a_object,
	std::string_view a_variableName,
	std::int32_t a_value)
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetSInt(a_value);
}

auto ScriptObject::GetFloat(ScriptObjectPtr a_object, std::string_view a_variableName) -> float
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	return variable ? variable->GetFloat() : 0.0f;
}

void ScriptObject::SetFloat(
	ScriptObjectPtr a_object,
	std::string_view a_variableName,
	float a_value)
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetFloat(a_value);
}

auto ScriptObject::GetString(ScriptObjectPtr a_object, std::string_view a_variableName)
	-> std::string
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	return variable ? std::string{ variable->GetString() } : ""s;
}

void ScriptObject::SetString(
	ScriptObjectPtr a_object,
	std::string_view a_variableName,
	std::string_view a_value)
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetString(a_value);
}

auto ScriptObject::GetArray(ScriptObjectPtr a_object, std::string_view a_variableName)
	-> ScriptArrayPtr
{
	auto variable = Utils::GetVariable(a_object, a_variableName);
	return variable ? variable->GetArray() : nullptr;
}

void ScriptObject::RegisterForModEvent(
	ScriptObjectPtr a_object,
	RE::BSFixedString a_eventName,
	RE::BSFixedString a_callbackName)
{
	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;

	if (vm)
	{
		auto args = RE::MakeFunctionArguments(std::move(a_eventName), std::move(a_callbackName));
		ScriptCallbackPtr nullCallback;
		vm->DispatchMethodCall1(a_object, "RegisterForModEvent", args, nullCallback);
		delete args;
	}
}

void ScriptObject::UnregisterForModEvent(
	ScriptObjectPtr a_object,
	RE::BSFixedString a_eventName)
{
	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;

	if (vm)
	{
		auto args = RE::MakeFunctionArguments(std::move(a_eventName));
		ScriptCallbackPtr nullCallback;
		vm->DispatchMethodCall1(a_object, "UnregisterForModEvent", args, nullCallback);
		delete args;
	}
}