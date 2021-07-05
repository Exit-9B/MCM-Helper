#pragma once

namespace ScriptObject
{
	auto GetBool(ScriptObjectPtr a_object, std::string_view a_variableName) -> bool;

	void SetBool(ScriptObjectPtr a_object, std::string_view a_variableName, bool a_value);

	auto GetInt(ScriptObjectPtr a_object, std::string_view a_variableName) -> std::int32_t;

	void SetInt(ScriptObjectPtr a_object, std::string_view a_variableName, std::int32_t a_value);

	auto GetFloat(ScriptObjectPtr a_object, std::string_view a_variableName) -> float;

	void SetFloat(ScriptObjectPtr a_object, std::string_view a_variableName, float a_value);

	auto GetString(ScriptObjectPtr a_object, std::string_view a_variableName) -> std::string;

	void SetString(
		ScriptObjectPtr a_object,
		std::string_view a_variableName,
		std::string_view a_value);

	auto GetArray(ScriptObjectPtr a_object, std::string_view a_variableName) -> ScriptArrayPtr;

	void RegisterForModEvent(
		ScriptObjectPtr a_object,
		RE::BSFixedString a_eventName,
		RE::BSFixedString a_callbackName);

	void UnregisterForModEvent(
		ScriptObjectPtr a_object,
		RE::BSFixedString a_eventName);
}