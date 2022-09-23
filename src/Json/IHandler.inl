#pragma once
#include "Json/IHandler.h"

template <typename... Args>
bool IHandler::ReportError(ErrorType errType, Args&&... args)
{
	std::string_view message;

	switch (errType) {
	case ErrorType::UnexpectedNull:
		message = "Unexpected null"sv;
		break;
	case ErrorType::UnexpectedValueType:
		message = "Unexpected {}: {}"sv;
		break;
	case ErrorType::UnexpectedStartObject:
		message = "Unexpected object: {}"sv;
		break;
	case ErrorType::UnexpectedKey:
		message = "Unexpected key: {}"sv;
		break;
	case ErrorType::UnexpectedEndObject:
		message = "Unexpected end of object with {} members"sv;
		break;
	case ErrorType::UnexpectedStartArray:
		message = "Unexpected array"sv;
		break;
	case ErrorType::UnexpectedEndArray:
		message = "Unexpected end of array with {} elements"sv;
		break;
	case ErrorType::InvalidKey:
		message = "Invalid key: {}"sv;
		break;
	case ErrorType::InvalidValue:
		message = "Invalid value: {}"sv;
		break;
	case ErrorType::MissingRequiredField:
		message = "Missing required field: {}"sv;
		break;
	}

	return ReportError(message, std::forward<Args>(args)...);
}

template <typename... Args>
bool IHandler::ReportError(std::string_view message, Args&&... args)
{
	std::string error = fmt::format(std::move(message), std::forward<Args>(args)...);
	_master->SetError(error);
	logger::warn(std::move(error));
	return false;
}
