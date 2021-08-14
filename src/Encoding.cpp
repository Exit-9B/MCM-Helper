#include "Encoding.h"

#include <Windows.h>
#include <stringapiset.h>

auto Encoding::Utf8ToUtf16(const std::string& a_utf8) -> std::wstring
{
	std::wstring utf16;
	if (a_utf8.empty()) {
		return utf16;
	}

	constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;

	if (a_utf8.length() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
		logger::warn("Input string too long to convert"sv);
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

auto Encoding::Utf16ToUtf8(const std::wstring& a_utf16) -> std::string
{
	std::string utf8;
	if (a_utf16.empty()) {
		return utf8;
	}

	constexpr DWORD kFlags = 0;

	if (a_utf16.length() > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
		logger::warn("Input string too long to convert"sv);
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
