#include "Script/ScriptObject.h"
#include "FormUtil.h"

auto ScriptObject::FromForm(RE::TESForm* a_form, const std::string& a_scriptName)
	-> ScriptObjectPtr
{
	if (!a_form) {
		logger::warn("Cannot retrieve script object from a None form."sv);

		return nullptr;
	}

	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	auto vm = skyrimVM ? skyrimVM->impl : nullptr;
	if (!vm) {
		return nullptr;
	}

	auto typeID = static_cast<RE::VMTypeID>(a_form->GetFormType());
	auto handle = skyrimVM->handlePolicy.GetHandleForObject(typeID, a_form);

	ScriptObjectPtr object;
	vm->FindBoundObject(handle, a_scriptName.c_str(), object);

	if (!object) {
		std::string formIdentifier = FormUtil::GetIdentifierFromForm(a_form);
		logger::warn("Script {} is not attached to form. {}"sv, a_scriptName, formIdentifier);
	}

	return object;
}

auto ScriptObject::GetVariable(ScriptObjectPtr a_object, std::string_view a_variableName)
	-> RE::BSScript::Variable*
{
	constexpr auto INVALID = static_cast<std::uint32_t>(-1);
	auto idx = INVALID;
	decltype(idx) offset = 0;
	for (auto cls = a_object->type.get(); cls; cls = cls->GetParent()) {
		const auto vars = cls->GetVariableIter();
		if (idx == INVALID) {
			if (vars) {
				for (std::uint32_t i = 0; i < cls->GetNumVariables(); i++) {
					const auto& var = vars[i];
					if (var.name == a_variableName) {
						idx = i;
						break;
					}
				}
			}
		}
		else {
			offset += cls->GetNumVariables();
		}
	}

	if (idx == INVALID) {
		logger::warn(
			"Variable {} does not exist on script {}"sv,
			a_variableName,
			a_object->GetTypeInfo()->GetName());

		return nullptr;
	}

	return std::addressof(a_object->variables[offset + idx]);
}

auto ScriptObject::IsType(ScriptObjectPtr a_object, const char* a_scriptName) -> bool
{
	for (auto cls = a_object ? a_object->type.get() : nullptr; cls; cls = cls->GetParent()) {
		if (_stricmp(cls->GetName(), a_scriptName) == 0) {
			return true;
		}
	}

	return false;
}

auto ScriptObject::GetBool(ScriptObjectPtr a_object, std::string_view a_variableName) -> bool
{
	auto variable = GetVariable(a_object, a_variableName);
	return variable ? variable->GetBool() : false;
}

void ScriptObject::SetBool(ScriptObjectPtr a_object, std::string_view a_variableName, bool a_value)
{
	auto variable = GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetBool(a_value);
}

auto ScriptObject::GetInt(ScriptObjectPtr a_object, std::string_view a_variableName)
	-> std::int32_t
{
	auto variable = GetVariable(a_object, a_variableName);
	return variable ? variable->GetSInt() : 0;
}

void ScriptObject::SetInt(
	ScriptObjectPtr a_object,
	std::string_view a_variableName,
	std::int32_t a_value)
{
	auto variable = GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetSInt(a_value);
}

auto ScriptObject::GetFloat(ScriptObjectPtr a_object, std::string_view a_variableName) -> float
{
	auto variable = GetVariable(a_object, a_variableName);
	return variable ? variable->GetFloat() : 0.0f;
}

void ScriptObject::SetFloat(
	ScriptObjectPtr a_object,
	std::string_view a_variableName,
	float a_value)
{
	auto variable = GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetFloat(a_value);
}

auto ScriptObject::GetString(ScriptObjectPtr a_object, std::string_view a_variableName)
	-> std::string
{
	auto variable = GetVariable(a_object, a_variableName);
	return variable ? std::string{ variable->GetString() } : ""s;
}

void ScriptObject::SetString(
	ScriptObjectPtr a_object,
	std::string_view a_variableName,
	std::string_view a_value)
{
	auto variable = GetVariable(a_object, a_variableName);
	if (variable)
		variable->SetString(a_value);
}

auto ScriptObject::GetArray(ScriptObjectPtr a_object, std::string_view a_variableName)
	-> ScriptArrayPtr
{
	auto variable = GetVariable(a_object, a_variableName);
	return variable ? variable->GetArray() : nullptr;
}

void ScriptObject::RegisterForModEvent(
	ScriptObjectPtr a_object,
	RE::BSFixedString a_eventName,
	RE::BSFixedString a_callbackName)
{
	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;

	if (vm) {
		auto args = RE::MakeFunctionArguments(std::move(a_eventName), std::move(a_callbackName));
		ScriptCallbackPtr nullCallback;
		vm->DispatchMethodCall(a_object, "RegisterForModEvent"sv, args, nullCallback);
		delete args;
	}
}

void ScriptObject::UnregisterForModEvent(ScriptObjectPtr a_object, RE::BSFixedString a_eventName)
{
	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;

	if (vm) {
		auto args = RE::MakeFunctionArguments(std::move(a_eventName));
		ScriptCallbackPtr nullCallback;
		vm->DispatchMethodCall(a_object, "UnregisterForModEvent"sv, args, nullCallback);
		delete args;
	}
}
