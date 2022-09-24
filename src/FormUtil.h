#pragma once

namespace FormUtil
{
	auto GetFormFromIdentifier(const std::string& a_identifier) -> RE::TESForm*;

	auto GetIdentifierFromForm(RE::TESForm* a_form) -> std::string;

	auto GetModName(RE::TESForm* a_form) -> std::string;
}
