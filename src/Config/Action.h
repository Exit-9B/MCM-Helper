#pragma once

class Action
{
public:
	using VM = RE::BSScript::IVirtualMachine;

	virtual void SendControlEvent(bool a_up, float a_holdTime = 0.0f) = 0;
};

class Function : public Action
{
public:
	using FunctionParam = std::variant<std::int32_t, bool, float, std::string>;
	class FunctionArguments : public RE::BSScript::IFunctionArguments
	{
	public:
		RE::BSScrapArray<RE::BSScript::Variable> Args;
		virtual bool operator()(RE::BSScrapArray<RE::BSScript::Variable>& a_dst) const override;

		static auto Make(std::span<std::string> a_params, FunctionParam a_value)
			-> std::unique_ptr<FunctionArguments>;
	};

	virtual void SendControlEvent(bool a_up, float a_holdTime) override;
	virtual VMAwaitable Invoke(VM* a_vm, FunctionParam a_value = "{value}"s) = 0;

	std::vector<std::string> Params;
	std::string Function;
	std::string ScriptName;
};

class CallFunction : public Function
{
public:
	VMAwaitable Invoke(VM* a_vm, FunctionParam a_value) override;

	RE::TESForm* Form;
};

class CallGlobalFunction : public Function
{
public:
	VMAwaitable Invoke(VM* a_vm, FunctionParam a_value) override;
};

class SendEvent : public Action
{
public:
	virtual void SendControlEvent(bool a_up, float a_holdTime) override;

	std::string Control;
	RE::TESForm* Form;
	std::string ScriptName;
};

class RunConsoleCommand : public Action
{
public:
	virtual void SendControlEvent(bool a_up, float a_holdTime) override;

	std::string Command;
};
