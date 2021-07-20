#pragma once

class Action
{
public:
	using VM = RE::BSScript::IVirtualMachine;

	class FunctionArguments : public RE::BSScript::IFunctionArguments
	{
	public:
		RE::BSScrapArray<RE::BSScript::Variable> Args;
		virtual bool operator()(RE::BSScrapArray<RE::BSScript::Variable>& a_dst) const override;

		template <typename T>
		inline static auto Make(std::span<std::string> a_params, T a_value)
			-> std::unique_ptr<FunctionArguments>;
	};

	virtual void InvokeBool(VM* a_vm, bool a_value) = 0;
	virtual void InvokeInt(VM* a_vm, std::int32_t a_value) = 0;
	virtual void InvokeFloat(VM* a_vm, float a_value) = 0;
	virtual void InvokeString(VM* a_vm, std::string_view a_value) = 0;

	void Invoke(VM* a_vm);

	std::vector<std::string> Params;
	std::string Function;
	std::string ScriptName;
};

class CallFunction : public Action
{
public:
	template <typename T>
	inline void Invoke(VM* a_vm, T a_value);

	virtual void InvokeBool(VM* a_vm, bool a_value) override;
	virtual void InvokeInt(VM* a_vm, std::int32_t a_value) override;
	virtual void InvokeFloat(VM* a_vm, float a_value) override;
	virtual void InvokeString(VM* a_vm, std::string_view a_value) override;

	RE::TESForm* Form;
};

class CallGlobalFunction : public Action
{
public:
	template <typename T>
	inline void Invoke(VM* a_vm, T a_value);

	virtual void InvokeBool(VM* a_vm, bool a_value) override;
	virtual void InvokeInt(VM* a_vm, std::int32_t a_value) override;
	virtual void InvokeFloat(VM* a_vm, float a_value) override;
	virtual void InvokeString(VM* a_vm, std::string_view a_value) override;
};

#include "Action.inl"