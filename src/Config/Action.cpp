#include "Config/Action.h"
#include "Config/Action.inl"

#include "ConfigPageCache.h"

bool Function::FunctionArguments::operator()(RE::BSScrapArray<RE::BSScript::Variable>& a_dst) const
{
	a_dst = Args;
	return true;
}

void Function::SendControlEvent(bool a_up, [[maybe_unused]] float a_holdTime)
{
	if (a_up)
		return;

	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;
	if (vm)
	{
		Invoke(vm.get());
	}
}

void Function::Invoke(VM* a_vm)
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

void SendEvent::SendControlEvent(bool a_up, float a_holdTime)
{
	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;

	if (vm)
	{
		auto typeID = static_cast<RE::VMTypeID>(Form->GetFormType());
		auto handle = skyrimVM->handlePolicy.GetHandleForObject(typeID, Form);

		if (handle)
		{
			RE::BSFixedString control{ Control };
			auto fnName = a_up ? "OnControlUp"sv : "OnControlDown"sv;

			auto args =
				a_up ? RE::MakeFunctionArguments(std::move(control), std::move(a_holdTime))
				: RE::MakeFunctionArguments(std::move(control));

			ScriptCallbackPtr nullCallback;
			vm->DispatchMethodCall(handle, ScriptName, fnName, args, nullCallback);
			delete args;
		}
	}
}

void RunConsoleCommand::SendControlEvent(bool a_up, [[maybe_unused]] float a_holdTime)
{
	if (a_up)
		return;

	const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
	const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
	if (script)
	{
		script->SetCommand(Command);
		script->CompileAndRun(nullptr);
		delete script;
	}
}