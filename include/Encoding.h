#pragma once

namespace Encoding
{
	auto Utf8ToUtf16(const std::string& a_utf8) -> std::wstring;

	auto Utf16ToUtf8(const std::wstring& a_utf16) -> std::string;
}
