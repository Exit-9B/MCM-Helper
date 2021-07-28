#include "Config/Action.h"
#include "ConfigPageCache.h"
#include "ColorUtil.h"
#include "Utils.h"

auto Action::FunctionArguments::Make(std::span<std::string> a_params, FunctionParam a_value)
	-> std::unique_ptr<Action::FunctionArguments>
{
	auto args = std::make_unique<FunctionArguments>();
	args->Args.resize(static_cast<std::uint32_t>(a_params.size()));
	for (std::uint32_t i = 0; i < a_params.size(); i++)
	{
		auto& var = args->Args[i];
		auto& param = a_params[i];
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

bool Action::FunctionArguments::operator()(RE::BSScrapArray<RE::BSScript::Variable>& a_dst) const
{
	a_dst = Args;
	return true;
}

void CallFunction::Invoke(VM* a_vm, FunctionParam a_value)
{
	if (!a_vm || Function.empty())
		return;

	auto object = Utils::GetScriptObject(Form, ScriptName);
	if (object)
	{
		auto args = FunctionArguments::Make(Params, a_value);

		ScriptCallbackPtr nullCallback;
		a_vm->DispatchMethodCall(object, Function, args.get(), nullCallback);
	}
}

void CallGlobalFunction::Invoke(VM* a_vm, FunctionParam a_value)
{
	if (!a_vm || Function.empty())
		return;

	if (!ScriptName.empty())
	{
		auto args = FunctionArguments::Make(Params, a_value);

		ScriptCallbackPtr nullCallback;
		a_vm->DispatchStaticCall(ScriptName, Function, args.get(), nullCallback);
	}
}