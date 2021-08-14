#pragma once

namespace Translation
{
	auto GetKeyName(std::uint32_t a_keyCode) -> std::string;

	auto ScaleformTranslate(const std::string& a_key) -> std::string;
}
