#pragma once

// ARGBColor from SKSE, can be manipulated via ColorComponent script
inline constexpr std::uint32_t MakeColor(
	std::uint8_t a,
	std::uint8_t r,
	std::uint8_t g,
	std::uint8_t b)
{
	return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | ((b & 0xFF) << 0);
}

[[nodiscard]] inline std::uint32_t PackARGB(const RE::Color& color)
{
	return MakeColor(color.alpha, color.red, color.green, color.blue);
}

[[nodiscard]] inline RE::Color UnpackARGB(std::uint32_t a_color)
{
	return RE::Color{
		(a_color >> 16) & 0xFF,
		(a_color >> 8) & 0xFF,
		(a_color >> 0) & 0xFF,
		(a_color >> 24) & 0xFF
	};
}

inline std::istream& operator>>(std::istream& _Istr, RE::Color& _Val)
{
	std::string token;

	std::getline(_Istr, token, ',');
	std::stringstream{ token } >> _Val.red;

	std::getline(_Istr, token, ',');
	std::stringstream{ token } >> _Val.green;

	std::getline(_Istr, token, ',');
	std::stringstream{ token } >> _Val.blue;

	return _Istr;
}

namespace std
{
	inline std::string to_string(const RE::Color& _Val)
	{
		std::stringstream ssColor;
		ssColor << static_cast<std::uint32_t>(_Val.red) << ","
			<< static_cast<std::uint32_t>(_Val.green) << ","
			<< static_cast<std::uint32_t>(_Val.blue);

		return ssColor.str();
	}
}
