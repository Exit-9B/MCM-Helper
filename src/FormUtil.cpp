#include "FormUtil.h"
#include "ConfigStore.h"

RE::TESForm* FormUtil::GetFormFromIdentifier(const std::string& a_identifier)
{
	std::istringstream ss{ a_identifier };
	std::string plugin, id;

	std::getline(ss, plugin, '|');
	std::getline(ss, id);
	RE::FormID relativeID;
	std::istringstream(id) >> std::hex >> relativeID;
	const auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (g_mergeMapperInterface) {
		const auto [mergedModName, mergedFormID] = g_mergeMapperInterface->GetNewFormID(plugin.c_str(), relativeID);
		std::string conversion_log = "";
		if (relativeID && mergedFormID && relativeID != mergedFormID) {
			conversion_log = std::format("0x{:x}->0x{:x}", relativeID, mergedFormID);
			relativeID = mergedFormID;
		}
		const std::string mergedModString{ mergedModName };
		if (!(plugin.empty()) && !mergedModString.empty() && plugin != mergedModString)
			{
			if (conversion_log.empty())
				conversion_log = std::format("{}->{}", plugin, mergedModString);
			else
				conversion_log = std::format("{}~{}->{}", conversion_log, plugin, mergedModString);
			plugin = mergedModName;
		}
		if (!conversion_log.empty())
			logger::debug("\t\tFound merged: {}", conversion_log);
	}
	return dataHandler ? dataHandler->LookupForm(relativeID, plugin) : nullptr;
}

std::string FormUtil::GetIdentifierFromForm(const RE::TESForm* a_form)
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

std::string FormUtil::GetModName(const RE::TESForm* a_form)
{
	if (!a_form)
		return ""s;

	const auto array = a_form->sourceFiles.array;
	if (!array || array->empty()) {
		return ""s;
	}

	const auto file = array->front();
	const auto filename = file ? file->GetFilename() : ""sv;
	auto modName = std::filesystem::path(filename).stem().string();
	if (g_mergeMapperInterface) {
		modName = std::filesystem::path(filename).filename().string();
		auto formID = a_form->GetFormID() & 0x00FFFFFF;
		auto name = a_form->GetName();
		auto editorID = a_form->GetFormEditorID();
		logger::debug("Attempting to find original mod for {} ({:x}) {}", name, formID, editorID);
		const auto [mergedModName, mergedFormID] = g_mergeMapperInterface->GetOriginalFormID(
			modName.c_str(),
			formID);
		std::string conversion_log = "";
		if (formID && mergedFormID && formID != mergedFormID) {
			conversion_log = std::format("0x{:x}->0x{:x}", formID, mergedFormID);
			formID = mergedFormID;
		}
		const std::string mergedModString{ mergedModName };
		if (!(modName.empty()) && !mergedModString.empty() && modName != mergedModString) {
			if (conversion_log.empty())
				conversion_log = std::format("{}->{}", modName, mergedModString);
			else
				conversion_log = std::format(
					"{}~{}->{}",
					conversion_log,
					modName,
					mergedModString);
			modName = mergedModString;
		}
		modName = modName.substr(0, modName.find(".es"));
		if (!conversion_log.empty())
			logger::debug("\t\tFound original: {}", conversion_log);
		else {
			ConfigStore::GetInstance().findMergedModName(modName);
		}
	}
	return modName;
}
