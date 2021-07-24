#pragma once

#include "Config/Action.h"
#include "ColorUtil.h"
#include "Utils.h"

template <typename T>
auto Function::FunctionArguments::Make(std::span<std::string> a_params, T a_value)
	-> std::unique_ptr<Function::FunctionArguments>
{
	auto args = std::make_unique<FunctionArguments>();
	args->Args.resize(static_cast<std::uint32_t>(a_params.size()));
	for (std::uint32_t i = 0; i < a_params.size(); i++)
	{
		auto& var = args->Args[i];
		auto& param = a_params[i];
		if (param == "{value}") {
			if constexpr (std::is_same_v<T, bool>)
			{
				var.SetBool(a_value);
			}
			else if constexpr (std::is_same_v<T, std::int32_t>)
			{
				var.SetSInt(a_value);
			}
			else if constexpr (std::is_same_v<T, float>)
			{
				var.SetFloat(a_value);
			}
			else if constexpr (std::is_convertible_v<T, std::string_view>)
			{
				var.SetString(a_value);
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

template <typename T>
void CallFunction::Invoke(VM* a_vm, T a_value)
{
	if (!a_vm || Function.empty())
		return;

	auto object = Utils::GetScriptObject(Form, ScriptName);
	if (object)
	{
		auto args = FunctionArguments::Make<T>(Params, a_value);

		ScriptCallbackPtr nullCallback;
		a_vm->DispatchMethodCall(object, Function, args.get(), nullCallback);
	}
}

template <typename T>
void CallGlobalFunction::Invoke(VM* a_vm, T a_value)
{
	if (!a_vm || Function.empty())
		return;

	if (!ScriptName.empty())
	{
		auto args = FunctionArguments::Make<T>(Params, a_value);

		ScriptCallbackPtr nullCallback;
		a_vm->DispatchStaticCall(ScriptName, Function, args.get(), nullCallback);
	}
}