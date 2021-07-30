#pragma once

namespace Utils
{
	auto GetFormFromIdentifier(const std::string& a_identifier) -> RE::TESForm*;

	auto GetIdentifierFromForm(RE::TESForm* a_form) -> std::string;

	auto GetModName(RE::TESForm* a_form) -> std::string;
}