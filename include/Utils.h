#pragma once

namespace Utils
{
	auto GetFormFromIdentifier(const std::string& a_identifier) -> RE::TESForm*;

	auto GetIdentifierFromForm(RE::TESForm* a_form) -> std::string;

	auto GetModName(RE::TESForm* a_form) -> std::string;

	auto GetKeyName(std::uint32_t a_keyCode) -> std::string;

	auto ScaleformTranslate(const std::string& a_key) -> std::string;

	auto Utf8ToUtf16(const std::string& a_utf8) -> std::wstring;

	auto Utf16ToUtf8(const std::wstring& a_utf16) -> std::string;
}
