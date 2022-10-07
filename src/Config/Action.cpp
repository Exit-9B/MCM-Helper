#include "Config/Action.h"
#include "ConfigPageCache.h"
#include "ColorUtil.h"
#include "Script/ScriptObject.h"

auto Function::FunctionArguments::Make(
	std::span<const std::string> a_params,
	FunctionParam a_value) -> std::unique_ptr<Function::FunctionArguments>
{
	auto args = std::make_unique<FunctionArguments>();
	args->Args.resize(static_cast<std::uint32_t>(a_params.size()));
	for (std::uint32_t i = 0; i < a_params.size(); i++) {
		auto& var = args->Args[i];
		const auto& param = a_params[i];
		if (param == "{value}") {
			if (std::holds_alternative<std::int32_t>(a_value)) {
				var.SetSInt(std::get<std::int32_t>(a_value));
			}
			else if (std::holds_alternative<bool>(a_value)) {
				var.SetBool(std::get<bool>(a_value));
			}
			else if (std::holds_alternative<float>(a_value)) {
				var.SetFloat(std::get<float>(a_value));
			}
			else if (std::holds_alternative<std::string>(a_value)) {
				var.SetString(std::get<std::string>(a_value));
			}
		}
		else if (param.rfind("{i}", 0) == 0) {
			std::istringstream ss{ param.substr(3) };
			std::int32_t value;
			ss >> value;
			var.SetSInt(value);
		}
		else if (param.rfind("{u}", 0) == 0) {
			std::istringstream ss{ param.substr(3) };
			std::uint32_t value;
			ss >> value;
			var.SetUInt(value);
		}
		else if (param.rfind("{b}", 0) == 0) {
			std::istringstream ss{ param.substr(3) };
			bool value;
			ss >> value;
			var.SetBool(value);
		}
		else if (param.rfind("{f}", 0) == 0) {
			std::istringstream ss{ param.substr(3) };
			float value;
			ss >> value;
			var.SetFloat(value);
		}
		else if (param.rfind("{s}", 0) == 0) {
			var.SetString(param.substr(3));
		}
		else if (param.rfind("{r}", 0) == 0) {
			std::istringstream ss{ param.substr(3) };
			RE::Color value;
			ss >> value;
			var.SetUInt(PackARGB(value));
		}
		else {
			var.SetString(param);
		}
	}
	return args;
}

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

	if (vm) {
		Invoke(vm.get());
	}
}

VMAwaitable CallFunction::Invoke(RE::BSScript::IVirtualMachine* a_vm, FunctionParam a_value)
{
	if (!a_vm || Function.empty())
		return {};

	auto object = ScriptObject::FromForm(Form, ScriptName);
	if (!object)
		return {};

	auto args = FunctionArguments::Make(Params, a_value);
	return a_vm->DispatchMethodCall(object, Function, args.get());
}

VMAwaitable CallGlobalFunction::Invoke(RE::BSScript::IVirtualMachine* a_vm, FunctionParam a_value)
{
	if (!a_vm || Function.empty() || ScriptName.empty())
		return {};

	auto args = FunctionArguments::Make(Params, a_value);
	return a_vm->DispatchStaticCall(ScriptName, Function, args.get());
}

void SendEvent::SendControlEvent(bool a_up, float a_holdTime)
{
	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;
	auto object = ScriptObject::FromForm(Form, ScriptName);

	if (!vm || !object)
		return;

	RE::BSFixedString control{ Control };
	const auto fnName = a_up ? "OnControlUp"sv : "OnControlDown"sv;

	ScriptArgs args{
		a_up ? RE::MakeFunctionArguments(std::move(control), std::move(a_holdTime))
			 : RE::MakeFunctionArguments(std::move(control))
	};

	vm->DispatchMethodCall(object, fnName, args.get());
}

void RunConsoleCommand::SendControlEvent(bool a_up, [[maybe_unused]] float a_holdTime)
{
	if (a_up)
		return;

	if (const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>()) {
		if (const std::unique_ptr<RE::Script> script{ scriptFactory->Create() }) {
			script->SetCommand(Command);
			script->CompileAndRun(nullptr);
		}
	}
}
