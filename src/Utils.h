#pragma once

namespace Utils
{
	// Form Management
	RE::TESForm* GetFormFromIdentifier(const std::string& a_identifier);

	std::string GetIdentifierFromForm(RE::TESForm* a_form);

	std::string GetModName(RE::TESForm* a_form);

	// Papyrus
	auto GetScriptObject(
		RE::TESForm* a_form,
		const char* a_scriptName) -> ScriptObjectPtr;

	RE::BSScript::Variable* GetScriptVariable(
		RE::TESForm* a_form,
		const char* a_scriptName,
		std::string_view a_variableName);

	RE::BSScript::Variable* GetVariable(
		ScriptObjectPtr a_object,
		std::string_view a_variableName);
}