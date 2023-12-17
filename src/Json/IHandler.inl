#pragma once
#include "Json/IHandler.h"

inline constexpr std::string_view IHandler::GetErrorMessage(ErrorType errType)
{
	switch (errType) {
	case ErrorType::UnexpectedNull:
		return "Unexpected null"sv;
	case ErrorType::UnexpectedValueType:
		return "Unexpected {}: {}"sv;
	case ErrorType::UnexpectedStartObject:
		return "Unexpected object: {}"sv;
	case ErrorType::UnexpectedKey:
		return "Unexpected key: {}"sv;
	case ErrorType::UnexpectedEndObject:
		return "Unexpected end of object with {} members"sv;
	case ErrorType::UnexpectedStartArray:
		return "Unexpected array"sv;
	case ErrorType::UnexpectedEndArray:
		return "Unexpected end of array with {} elements"sv;
	case ErrorType::InvalidKey:
		return "Invalid key: {}"sv;
	case ErrorType::InvalidValue:
		return "Invalid value: {}"sv;
	case ErrorType::MissingRequiredField:
		return "Missing required field: {}"sv;
	default:
		return "Unknown error"sv;
	}
}

template <typename... Args>
inline bool IHandler::ReportError(ErrorType errType, Args&&... args)
{
	return ReportError(GetErrorMessage(errType), std::forward<Args>(args)...);
}

template <typename... Args>
inline bool IHandler::ReportError(std::string_view message, Args&&... args)
{
	std::string error = fmt::vformat(message, fmt::make_format_args(args...));
	_master->SetError(error);
	logger::warn("{}"sv, error);

	return false;
}
