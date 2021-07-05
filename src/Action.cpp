#include "Action.h"
#include "Action.inl"

#include "ConfigPageCache.h"

bool Action::FunctionArguments::operator()(RE::BSScrapArray<RE::BSScript::Variable>& a_dst) const
{
	a_dst = Args;
	return true;
}

void Action::Invoke(VM* a_vm)
{
	// Equivalent call; we have nothing to replace {value} with, so replace it with itself
	InvokeString(a_vm, "{value}"sv);
}

void CallFunction::InvokeBool(VM* a_vm, bool a_value)
{
	Invoke(a_vm, a_value);
}

void CallFunction::InvokeInt(VM* a_vm, std::int32_t a_value)
{
	Invoke(a_vm, a_value);
}

void CallFunction::InvokeFloat(VM* a_vm, float a_value)
{
	Invoke(a_vm, a_value);
}

void CallFunction::InvokeString(VM* a_vm, std::string_view a_value)
{
	Invoke(a_vm, a_value);
}

auto CallFunction::GetScriptObject() -> ScriptObjectPtr
{
	if (Form)
	{
		auto scriptName = !ScriptName.empty() ? ScriptName.c_str() : nullptr;
		return Utils::GetScriptObject(Form, scriptName);
	}
	else
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		auto form = configPageCache.GetCurrentForm();
		auto scriptName = configPageCache.GetCurrentScriptName();
		return Utils::GetScriptObject(form, scriptName);
	}
}

void CallGlobalFunction::InvokeBool(VM* a_vm, bool a_value)
{
	Invoke(a_vm, a_value);
}

void CallGlobalFunction::InvokeInt(VM* a_vm, std::int32_t a_value)
{
	Invoke(a_vm, a_value);
}

void CallGlobalFunction::InvokeFloat(VM* a_vm, float a_value)
{
	Invoke(a_vm, a_value);
}

void CallGlobalFunction::InvokeString(VM* a_vm, std::string_view a_value)
{
	Invoke(a_vm, a_value);
}

auto CallGlobalFunction::GetScriptName() -> std::string
{
	if (!ScriptName.empty())
	{
		return ScriptName;
	}
	else
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		return configPageCache.GetCurrentScriptName();
	}
}