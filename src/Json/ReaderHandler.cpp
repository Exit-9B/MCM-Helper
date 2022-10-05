#include "Json/ReaderHandler.h"
#include "Json/IHandler.h"
#include <rapidjson/filereadstream.h>

rapidjson::ParseResult ReaderHandler::ReadFile(const std::filesystem::path& path)
{
	::errno_t err = 0;
	std::unique_ptr<FILE, decltype(&::fclose)> fp{
		[&path, &err]
		{
			FILE* fp = nullptr;
			err = _wfopen_s(&fp, path.c_str(), L"r");
			return fp;
		}(),
		&::fclose
	};

	if (err != 0) {
		logger::warn("Failed to open file: {}", path.string());
		return { rapidjson::kParseErrorDocumentEmpty, 0 };
	}

	char readBuffer[65536]{};
	rapidjson::FileReadStream is{ fp.get(), readBuffer, sizeof(readBuffer) };
	rapidjson::Reader reader;

	return reader.Parse(is, *this);
}

void ReaderHandler::SetError(const std::string& error)
{
	_error = error;
}

const std::string& ReaderHandler::GetError() const
{
	return _error;
}

bool ReaderHandler::Null()
{
	return _handlers.top()->Null();
}

bool ReaderHandler::Bool(bool b)
{
	return _handlers.top()->Bool(b);
}

bool ReaderHandler::Int(int i)
{
	return _handlers.top()->Int(i);
}

bool ReaderHandler::Uint(unsigned i)
{
	return _handlers.top()->Uint(i);
}

bool ReaderHandler::Int64(std::int64_t i)
{
	return _handlers.top()->Int64(i);
}

bool ReaderHandler::Uint64(std::uint64_t i)
{
	return _handlers.top()->Uint64(i);
}

bool ReaderHandler::Double(double d)
{
	return _handlers.top()->Double(d);
}

bool ReaderHandler::RawNumber(const Ch* str, SizeType length, bool copy)
{
	return _handlers.top()->RawNumber(str, length, copy);
}

bool ReaderHandler::String(const Ch* str, SizeType length, bool copy)
{
	return _handlers.top()->String(str, length, copy);
}

bool ReaderHandler::StartObject()
{
	return _handlers.top()->StartObject();
}

bool ReaderHandler::Key(const Ch* str, SizeType length, bool copy)
{
	return _handlers.top()->Key(str, length, copy);
}

bool ReaderHandler::EndObject(SizeType memberCount)
{
	return _handlers.top()->EndObject(memberCount);
}

bool ReaderHandler::StartArray()
{
	return _handlers.top()->StartArray();
}

bool ReaderHandler::EndArray(SizeType elementCount)
{
	return _handlers.top()->EndArray(elementCount);
}
