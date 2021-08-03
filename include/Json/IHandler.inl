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
	case ErrorType::UnexpectedBool:
		message = "Unexpected boolean: {}"sv;
		break;
	case ErrorType::UnexpectedInt:
		message = "Unexpected integer: {}"sv;
		break;
	case ErrorType::UnexpectedUint:
		message = "Unexpected unsigned integer: {}"sv;
		break;
	case ErrorType::UnexpectedInt64:
		message = "Unexpected 64-bit integer: {}"sv;
		break;
	case ErrorType::UnexpectedUint64:
		message = "Unexpected 64-bit unsigned integer: {}"sv;
		break;
	case ErrorType::UnexpectedDouble:
		message = "Unexpected floating point: {}"sv;
		break;
	case ErrorType::UnexpectedNumber:
		message = "Unexpected number: {}"sv;
		break;
	case ErrorType::UnexpectedString:
		message = "Unexpected string: {}"sv;
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
	logger::warn(std::move(message), std::forward<Args>(args)...);
	return false;
}