#pragma once

namespace Utils
{
	// Form Management
	auto GetFormFromIdentifier(const std::string& a_identifier) -> RE::TESForm*;

	auto GetIdentifierFromForm(RE::TESForm* a_form) -> std::string;

	auto GetModName(RE::TESForm* a_form) -> std::string;

	// Papyrus
	auto GetScriptObject(
		RE::TESForm* a_form,
		const std::string& a_scriptName)
		-> ScriptObjectPtr;

	auto GetScriptProperty(
		RE::TESForm* a_form,
		const std::string& a_scriptName,
		std::string_view a_propertyName)
		-> RE::BSScript::Variable*;

	auto GetScriptVariable(
		RE::TESForm* a_form,
		const std::string& a_scriptName,
		std::string_view a_variableName)
		-> RE::BSScript::Variable*;

	auto GetVariable(
		ScriptObjectPtr a_object,
		std::string_view a_variableName)
		-> RE::BSScript::Variable*;
}