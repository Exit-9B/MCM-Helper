#include "Utils.h"
#include "Script/ScriptObject.h"
#include <Windows.h>
#include <stringapiset.h>

auto Utils::GetFormFromIdentifier(const std::string& a_identifier) -> RE::TESForm*
{
	std::istringstream ss{ a_identifier };
	std::string plugin, id;

	std::getline(ss, plugin, '|');
	std::getline(ss, id);
	RE::FormID relativeID;
	std::istringstream{ id } >> std::hex >> relativeID;
	const auto dataHandler = RE::TESDataHandler::GetSingleton();
	return dataHandler ? dataHandler->LookupForm(relativeID, plugin) : nullptr;
}

auto Utils::GetIdentifierFromForm(RE::TESForm* a_form) -> std::string
{
	if (!a_form) {
		return ""s;
	}

	auto file = a_form->GetFile();
	auto plugin = file ? file->fileName : "";

	RE::FormID formID = a_form->GetFormID();
	RE::FormID relativeID = formID & 0x00FFFFFF;

#ifndef SKYRIMVR
	if (file && file->recordFlags.all(RE::TESFile::RecordFlag::kSmallFile)) {
		relativeID &= 0x00000FFF;
	}
#endif

	std::ostringstream ss;
	ss << plugin << "|" << std::hex << relativeID;
	return ss.str();
}

auto Utils::GetModName(RE::TESForm* a_form) -> std::string
{
	auto file = a_form ? a_form->GetFile() : nullptr;
	auto fileName = file ? file->fileName : nullptr;

	return std::filesystem::path{ fileName }.stem().string();
}

auto Utils::ScaleformTranslate(const std::string& a_key) -> std::string
{
	if (!a_key.starts_with('$')) {
		return a_key;
	}

	const auto scaleformManager = RE::BSScaleformManager::GetSingleton();
	const auto loader = scaleformManager ? scaleformManager->loader : nullptr;
	const auto translator = loader ?
        loader->GetStateAddRef<RE::BSScaleformTranslator>(RE::GFxState::StateType::kTranslator) :
        nullptr;

	if (!translator) {
		logger::warn("Failed to get Scaleform translator"sv);
		return a_key;
	}

	std::string key;
	std::string nested;

	auto nestOpen = a_key.find_first_of('{');
	if (nestOpen != std::string::npos) {
		auto nestClose = a_key.find_last_of('}');
		if (nestClose != std::string::npos && nestClose > nestOpen) {
			key = a_key.substr(0, nestOpen + 1) +
				a_key.substr(nestClose, a_key.size() - nestClose);
			auto nestedKey = a_key.substr(nestOpen + 1, nestClose - nestOpen - 1);
			nested = ScaleformTranslate(nestedKey);
		}
	}

	if (key.empty()) {
		key = a_key;
	}

	auto key_utf16 = Utf8ToUtf16(key);
	RE::GFxWStringBuffer result;

	RE::GFxTranslator::TranslateInfo translateInfo;
	translateInfo.key = key_utf16.c_str();
	translateInfo.result = std::addressof(result);

	translator->Translate(std::addressof(translateInfo));

	auto result_utf8 = Utf16ToUtf8(result.c_str());
	return std::format(std::string_view{ result_utf8 }, nested);
}

auto Utils::Utf8ToUtf16(const std::string& a_utf8) -> std::wstring
{
	std::wstring utf16;
	if (a_utf8.empty()) {
		return utf16;
	}

	constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;

	if (a_utf8.length() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
		logger::warn("Input string too long to translate"sv);
		return utf16;
	}

	const int utf8Length = static_cast<int>(a_utf8.length());

	const int utf16Length = ::MultiByteToWideChar(
		CP_UTF8,
		kFlags,
		a_utf8.data(),
		utf8Length,
		nullptr,
		0);

	if (utf16Length == 0) {
		logger::warn("Cannot get result string length when converting from UTF-8 to UTF-16"sv);
		return utf16;
	}

	utf16.resize(utf16Length);

	int result = ::MultiByteToWideChar(
		CP_UTF8,
		kFlags,
		a_utf8.data(),
		utf8Length,
		std::addressof(utf16[0]),
		utf16Length);

	if (result == 0) {
		logger::warn("Cannot convert from UTF-8 to UTF-16"sv);
		return utf16;
	}

	return utf16;
}

auto Utils::Utf16ToUtf8(const std::wstring& a_utf16) -> std::string
{
	std::string utf8;
	if (a_utf16.empty()) {
		return utf8;
	}

	constexpr DWORD kFlags = 0;

	if (a_utf16.length() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
		logger::warn("Input string too long to translate"sv);
		return utf8;
	}

	const int utf16Length = static_cast<int>(a_utf16.length());

	const int utf8Length = ::WideCharToMultiByte(
		CP_UTF8,
		kFlags,
		a_utf16.data(),
		utf16Length,
		nullptr,
		0,
		nullptr,
		nullptr);

	if (utf8Length == 0) {
		logger::warn("Cannot get result string length when converting from UTF-16 to UTF-8"sv);
		return utf8;
	}

	utf8.resize(utf8Length);

	int result = ::WideCharToMultiByte(
		CP_UTF8,
		kFlags,
		a_utf16.data(),
		utf16Length,
		std::addressof(utf8[0]),
		utf8Length,
		nullptr,
		nullptr);

	if (result == 0) {
		logger::warn("Cannot convert from UTF-16 to UTF-8"sv);
		return utf8;
	}

	return utf8;
}
