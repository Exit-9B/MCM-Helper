#include "Utils.h"

RE::TESForm* Utils::GetFormFromIdentifier(const std::string& a_identifier)
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

std::string Utils::GetIdentifierFromForm(RE::TESForm* a_form)
{
	if (!a_form)
	{
		return ""s;
	}

	auto file = a_form->GetFile();
	auto plugin = file ? file->fileName : "";

	RE::FormID formID = a_form->GetFormID();
	RE::FormID relativeID = formID & 0x00FFFFFF;

	if (file && file->recordFlags.all(RE::TESFile::RecordFlag::kSmallFile))
	{
		relativeID &= 0x00000FFF;
	}

	std::ostringstream ss;
	ss << plugin << "|" << std::hex << relativeID;
	return ss.str();
}

std::string Utils::GetModName(RE::TESForm* a_form)
{
	auto file = a_form ? a_form->GetFile() : nullptr;
	auto fileName = file ? file->fileName : nullptr;

	return std::filesystem::path{ fileName }.stem().string();
}

auto Utils::GetScriptObject(RE::TESForm* a_form, const char* a_scriptName) -> ScriptObjectPtr
{
	if (!a_form)
	{
		logger::warn("Cannot retrieve script object from a None form."sv);

		return nullptr;
	}

	const auto skyrimVM = RE::SkyrimVM::GetSingleton();

	auto typeID = static_cast<RE::VMTypeID>(a_form->GetFormType());
	RE::VMHandle handle = skyrimVM->handlePolicy.GetHandleForObject(typeID, a_form);

	ScriptObjectPtr object;
	skyrimVM->impl->FindBoundObject(handle, a_scriptName, object);

	if (!object)
	{
		std::string formIdentifier = GetIdentifierFromForm(a_form);
		if (a_scriptName)
		{
			logger::warn(
				"Script {} is not attached to form. {}"sv,
				a_scriptName,
				formIdentifier);
		}
		else
		{
			logger::warn(
				"Cannot retrieve script object from a form with no scripts attached. {}"sv,
				formIdentifier);
		}
	}

	return object;
}

RE::BSScript::Variable* Utils::GetScriptVariable(
	RE::TESForm* a_form,
	const char* a_scriptName,
	std::string_view a_variableName)
{
	auto object = GetScriptObject(a_form, a_scriptName);
	if (!object)
	{
		return nullptr;
	}

	return GetVariable(object, a_variableName);
}

RE::BSScript::Variable* Utils::GetVariable(
	ScriptObjectPtr a_object,
	std::string_view a_variableName)
{
	constexpr auto INVALID = static_cast<std::uint32_t>(-1);
	auto idx = INVALID;
	decltype(idx) offset = 0;
	for (auto cls = a_object->type.get(); cls; cls = cls->GetParent())
	{
		const auto vars = cls->GetVariableIter();
		if (idx == INVALID)
		{
			if (vars)
			{
				for (std::uint32_t i = 0; i < cls->GetNumVariables(); i++)
				{
					const auto& var = vars[i];
					if (var.name == a_variableName)
					{
						idx = i;
						break;
					}
				}
			}
		}
		else
		{
			offset += cls->GetNumVariables();
		}
	}

	if (idx == INVALID)
	{
		logger::warn(
			"Variable {} does not exist on script {}"sv,
			a_variableName,
			a_object->GetTypeInfo()->GetName());

		return nullptr;
	}

	return std::addressof(a_object->variables[offset + idx]);
}