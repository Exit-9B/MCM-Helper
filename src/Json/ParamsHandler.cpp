#include "Json/ParamsHandler.h"

ParamsHandler::ParamsHandler(ReaderHandler* master, std::vector<std::string>* params) :
	IHandler{ master },
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
		return IHandler::Bool(b);
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
		return IHandler::Int(i);
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
		return IHandler::Uint(i);
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
		return IHandler::Double(d);
	}
}

bool ParamsHandler::String(const Ch* str, SizeType length, bool copy)
{
	switch (_state) {
	case State::Main:
		_params->push_back(str);
		return true;
	default:
		return IHandler::String(str, length, copy);
	}
}

bool ParamsHandler::StartArray()
{
	switch (_state) {
	case State::Init:
		_state = State::Main;
		return true;
	default:
		return IHandler::StartArray();
	}
}

bool ParamsHandler::EndArray(SizeType elementCount)
{
	switch (_state) {
	case State::Main:
		_master->PopHandler();
		return true;
	default:
		return IHandler::EndArray(elementCount);
	}
}
