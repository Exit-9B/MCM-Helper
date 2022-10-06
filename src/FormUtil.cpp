#include "FormUtil.h"

RE::TESForm* FormUtil::GetFormFromIdentifier(const std::string& a_identifier)
{
	std::istringstream ss{ a_identifier };
	std::string plugin, id;

	std::getline(ss, plugin, '|');
	std::getline(ss, id);
	RE::FormID relativeID;
	std::istringstream{ id } >> std::hex >> relativeID;
	const auto dataHandler = RE::TESDataHandler::GetSingleton();
	return dataHandler ? dataHandler->LookupForm(relativeID, plugin) : nullptr;
}

std::string FormUtil::GetIdentifierFromForm(RE::TESForm* a_form)
{
	if (!a_form) {
		return ""s;
	}

	auto file = a_form->GetFile();
	auto plugin = file ? file->fileName : "";

	RE::FormID formID = a_form->GetFormID();
	RE::FormID relativeID = formID & 0x00FFFFFF;

#ifndef SKYRIMVR
	if (file && file->recordFlags.all(RE::TESFile::RecordFlag::kSmallFile)) {
		relativeID &= 0x00000FFF;
	}
#endif

	std::ostringstream ss;
	ss << plugin << "|" << std::hex << relativeID;
	return ss.str();
}

std::string FormUtil::GetModName(RE::TESForm* a_form)
{
	auto file = a_form ? a_form->GetFile() : nullptr;
	auto fileName = file ? file->fileName : nullptr;

	return fileName ? std::filesystem::path{ fileName }.stem().string() : ""s;
}
