#pragma once

namespace FormUtil
{
	[[nodiscard]] RE::TESForm* GetFormFromIdentifier(const std::string& a_identifier);

	[[nodiscard]] std::string GetIdentifierFromForm(RE::TESForm* a_form);

	[[nodiscard]] std::string GetModName(RE::TESForm* a_form);
}
