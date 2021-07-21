#include "Papyrus/RegisterFuncs.h"
#include "Papyrus/MCM.h"
#include "Papyrus/MCM_ConfigBase.h"

bool Papyrus::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
{
	MCM::RegisterFuncs(a_vm);
	MCM_ConfigBase::RegisterFuncs(a_vm);

	return true;
}