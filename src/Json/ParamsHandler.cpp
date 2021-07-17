#include "Json/ParamsHandler.h"

ParamsHandler::ParamsHandler(ReaderHandler* master, std::vector<std::string>* params) :
	_master{ master },
	_params{ params }
{
}

bool ParamsHandler::Bool(bool b)
{
	switch (_state) {
	case State::Main:
	{
		std::ostringstream ss;
		ss << "{b}" << b;
		_params->push_back(ss.str());
		return true;
	}
	default:
		return false;
	}
}

bool ParamsHandler::Int(int i)
{
	switch (_state) {
	case State::Main:
	{
		std::ostringstream ss;
		ss << "{i}" << i;
		_params->push_back(ss.str());
		return true;
	}
	default:
		return false;
	}
}

bool ParamsHandler::Uint(unsigned i)
{
	switch (_state) {
	case State::Main:
	{
		std::ostringstream ss;
		ss << "{i}" << i;
		_params->push_back(ss.str());
		return true;
	}
	default:
		return false;
	}
}

bool ParamsHandler::Double(double d)
{
	switch (_state) {
	case State::Main:
	{
		std::ostringstream ss;
		ss << "{f}" << d;
		_params->push_back(ss.str());
		return true;
	}
	default:
		return false;
	}
}

bool ParamsHandler::String(
	const Ch* str,
	[[maybe_unused]] SizeType length,
	[[maybe_unused]] bool copy)
{
	switch (_state) {
	case State::Main:
		_params->push_back(str);
		return true;
	default:
		return false;
	}
}

bool ParamsHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return false;
	}
}

bool ParamsHandler::EndArray([[maybe_unused]] SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return false;
	}
}
