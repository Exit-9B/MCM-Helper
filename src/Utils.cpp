#include "Utils.h"

auto Utils::GetFormFromIdentifier(const std::string& a_identifier) -> RE::TESForm*
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

auto Utils::GetIdentifierFromForm(RE::TESForm* a_form) -> std::string
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

auto Utils::GetModName(RE::TESForm* a_form) -> std::string
{
	auto file = a_form ? a_form->GetFile() : nullptr;
	auto fileName = file ? file->fileName : nullptr;

	return std::filesystem::path{ fileName }.stem().string();
}

auto Utils::GetScriptObject(RE::TESForm* a_form, const std::string& a_scriptName) -> ScriptObjectPtr
{
	if (!a_form)
	{
		logger::warn("Cannot retrieve script object from a None form."sv);

		return nullptr;
	}

	const auto skyrimVM = RE::SkyrimVM::GetSingleton();

	auto typeID = static_cast<RE::VMTypeID>(a_form->GetFormType());
	RE::VMHandle handle = skyrimVM->handlePolicy.GetHandleForObject(typeID, a_form);
	auto scriptName = !a_scriptName.empty() ? a_scriptName.c_str() : nullptr;

	ScriptObjectPtr object;
	skyrimVM->impl->FindBoundObject(handle, scriptName, object);

	if (!object)
	{
		std::string formIdentifier = GetIdentifierFromForm(a_form);
		if (!a_scriptName.empty())
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

auto Utils::GetScriptProperty(
	RE::TESForm* a_form,
	const std::string& a_scriptName,
	std::string_view a_propertyName)
	-> RE::BSScript::Variable*
{
	auto scriptName = !a_scriptName.empty() ? a_scriptName.c_str() : nullptr;
	auto script = Utils::GetScriptObject(a_form, scriptName);
	return script ? script->GetProperty(a_propertyName) : nullptr;
}

RE::BSScript::Variable* Utils::GetScriptVariable(
	RE::TESForm* a_form,
	const std::string& a_scriptName,
	std::string_view a_variableName)
{
	auto object = GetScriptObject(a_form, a_scriptName);
	if (!object)
	{
		return nullptr;
	}

	return GetVariable(object, a_variableName);
}

auto Utils::GetVariable(
	ScriptObjectPtr a_object,
	std::string_view a_variableName)
	-> RE::BSScript::Variable*
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