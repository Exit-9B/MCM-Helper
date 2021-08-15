#include "Papyrus/MCM_ConfigBase.h"
#include "Script/SkyUI.h"
#include "Script/ScriptObject.h"
#include "ConfigStore.h"
#include "ConfigPageCache.h"
#include "KeybindManager.h"
#include "FormUtil.h"
#include "Translation.h"

#define REGISTER_FUNCTION(vm, func) vm->RegisterFunction(#func##sv, ScriptName, func)

namespace Papyrus
{
	void MCM_ConfigBase::RefreshMenu(RE::TESQuest* a_self)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);
		auto config = ConfigStore::GetInstance().GetConfig(a_self);

		if (config) {
			config->RefreshPage(object);
		}
	}

	void MCM_ConfigBase::SetMenuOptions(
		RE::TESQuest* a_self,
		std::string a_ID,
		std::vector<std::string> a_options,
		std::vector<std::string> a_shortNames)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		configPageCache.SetMenuOptions(a_ID, a_options, a_shortNames);
	}

	auto MCM_ConfigBase::GetModSettingInt(RE::TESQuest* a_self, std::string_view a_settingName)
		-> std::int32_t
	{
		auto modName = FormUtil::GetModName(a_self);
		return SettingStore::GetInstance().GetModSettingInt(modName, a_settingName);
	}

	auto MCM_ConfigBase::GetModSettingBool(RE::TESQuest* a_self, std::string_view a_settingName)
		-> bool
	{
		auto modName = FormUtil::GetModName(a_self);
		return SettingStore::GetInstance().GetModSettingBool(modName, a_settingName);
	}

	auto MCM_ConfigBase::GetModSettingFloat(RE::TESQuest* a_self, std::string_view a_settingName)
		-> float
	{
		auto modName = FormUtil::GetModName(a_self);
		return SettingStore::GetInstance().GetModSettingFloat(modName, a_settingName);
	}

	auto MCM_ConfigBase::GetModSettingString(RE::TESQuest* a_self, std::string_view a_settingName)
		-> std::string
	{
		auto modName = FormUtil::GetModName(a_self);
		auto value = SettingStore::GetInstance().GetModSettingString(modName, a_settingName);
		return value ? value : ""s;
	}

	void MCM_ConfigBase::SetModSettingInt(
		RE::TESQuest* a_self,
		std::string_view a_settingName,
		std::int32_t a_value)
	{
		auto modName = FormUtil::GetModName(a_self);
		SettingStore::GetInstance().SetModSettingInt(modName, a_settingName, a_value);
	}

	void MCM_ConfigBase::SetModSettingBool(
		RE::TESQuest* a_self,
		std::string_view a_settingName,
		bool a_value)
	{
		auto modName = FormUtil::GetModName(a_self);
		SettingStore::GetInstance().SetModSettingBool(modName, a_settingName, a_value);
	}

	void MCM_ConfigBase::SetModSettingFloat(
		RE::TESQuest* a_self,
		std::string_view a_settingName,
		float a_value)
	{
		auto modName = FormUtil::GetModName(a_self);
		SettingStore::GetInstance().SetModSettingFloat(modName, a_settingName, a_value);
	}

	void MCM_ConfigBase::SetModSettingString(
		RE::TESQuest* a_self,
		std::string_view a_settingName,
		std::string_view a_value)
	{
		auto modName = FormUtil::GetModName(a_self);
		SettingStore::GetInstance().SetModSettingString(modName, a_settingName, a_value);
	}

	void MCM_ConfigBase::OnPageReset(RE::TESQuest* a_self, std::string a_page)
	{
		auto object = ScriptObject::FromForm(a_self, ScriptName);
		auto config = ConfigStore::GetInstance().GetConfig(a_self);

		if (config) {
			config->ShowPage(object, a_page);
		}
	}

	void MCM_ConfigBase::OnOptionHighlight(RE::TESQuest* a_self, std::int32_t a_option)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		configPageCache.SetHighlight(a_option);
		UpdateInfoText(object, false);
	}

	void MCM_ConfigBase::OnOptionSelect(
		RE::BSScript::IVirtualMachine* a_vm,
		[[maybe_unused]] RE::VMStackID a_stackID,
		RE::TESQuest* a_self,
		std::int32_t a_option)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto toggle = std::dynamic_pointer_cast<ToggleControl>(control)) {
			if (toggle->ValueSource) {
				if (toggle->ValueSource->GetValue() == 0.0f) {
					toggle->ValueSource->SetValue(1.0f);
				}
				else {
					toggle->ValueSource->SetValue(0.0f);
				}
			}

			toggle->Refresh(object, a_option);

			if (toggle->GroupControl) {
				auto config = ConfigStore::GetInstance().GetConfig(a_self);
				if (config) {
					config->RefreshPage(object);
				}
			}

			SendSettingChangeEvent(a_vm, object, toggle->ID);
		}
		else if (auto stepper = std::dynamic_pointer_cast<StepperControl>(control)) {
			if (stepper->ValueSource && !stepper->Options.empty()) {
				std::int32_t index = static_cast<std::int32_t>(stepper->GetValue());
				std::int32_t nextIndex = (index + 1) % stepper->Options.size();
				stepper->ValueSource->SetValue(static_cast<float>(nextIndex));
			}

			stepper->Refresh(object, a_option);

			SendSettingChangeEvent(a_vm, object, stepper->ID);
		}
		else if (auto error = std::dynamic_pointer_cast<ErrorControl>(control)) {
			SkyUI::Config::ShowMessage(object, error->Error, false, nullptr);
		}

		if (control) {
			control->InvokeAction(a_vm);
		}
	}

	void MCM_ConfigBase::OnOptionDefault(RE::TESQuest* a_self, std::int32_t a_option)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (control) {
			control->ResetToDefault();
			control->Refresh(object, a_option);
		}
	}

	void MCM_ConfigBase::OnOptionSliderOpen(RE::TESQuest* a_self, std::int32_t a_option)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto slider = std::dynamic_pointer_cast<SliderControl>(control)) {
			if (slider->ValueSource) {
				auto startValue = slider->ValueSource->GetValue();
				SkyUI::Config::SetSliderDialogStartValue(object, startValue);

				auto defaultValue = slider->ValueSource->GetDefaultValue();
				SkyUI::Config::SetSliderDialogDefaultValue(object, defaultValue);
			}

			SkyUI::Config::SetSliderDialogRange(object, slider->Min, slider->Max);
			SkyUI::Config::SetSliderDialogInterval(object, slider->Step);
		}
	}

	void MCM_ConfigBase::OnOptionSliderAccept(
		RE::BSScript::IVirtualMachine* a_vm,
		[[maybe_unused]] RE::VMStackID a_stackID,
		RE::TESQuest* a_self,
		std::int32_t a_option,
		float a_value)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto slider = std::dynamic_pointer_cast<SliderControl>(control)) {
			if (slider->ValueSource) {
				slider->ValueSource->SetValue(a_value);
			}

			slider->Refresh(object, a_option);

			SendSettingChangeEvent(a_vm, object, slider->ID);

			slider->InvokeAction(a_vm);
		}
	}

	void MCM_ConfigBase::OnOptionMenuOpen(RE::TESQuest* a_self, std::int32_t a_option)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto menu = std::dynamic_pointer_cast<MenuControl>(control)) {
			auto options = configPageCache.GetMenuOptions(menu.get());
			auto item = std::find(options.begin(), options.end(), menu->GetValue());

			SkyUI::Config::SetMenuDialogOptions(object, options);
			if (item != options.end()) {
				auto index = static_cast<std::int32_t>(item - options.begin());
				SkyUI::Config::SetMenuDialogStartIndex(object, index);
			}

			auto defaultValue = menu->GetDefaultValue();
			if (!defaultValue.empty()) {
				auto defaultItem = std::find(options.begin(), options.end(), defaultValue);
				if (defaultItem != options.end()) {
					auto index = static_cast<std::int32_t>(defaultItem - options.begin());
					SkyUI::Config::SetMenuDialogDefaultIndex(object, index);
				}
			}
		}
		else if (auto menuEnum = std::dynamic_pointer_cast<EnumControl>(control)) {
			auto options = configPageCache.GetMenuOptions(menuEnum.get());
			auto index = menuEnum->GetValue();

			SkyUI::Config::SetMenuDialogOptions(object, options);
			SkyUI::Config::SetMenuDialogStartIndex(object, index);

			if (menuEnum->ValueSource) {
				auto defaultValue = static_cast<std::int32_t>(
					menuEnum->ValueSource->GetDefaultValue());
				SkyUI::Config::SetMenuDialogDefaultIndex(object, defaultValue);
			}
		}
	}

	void MCM_ConfigBase::OnOptionMenuAccept(
		RE::BSScript::IVirtualMachine* a_vm,
		[[maybe_unused]] RE::VMStackID a_stackID,
		RE::TESQuest* a_self,
		std::int32_t a_option,
		std::int32_t a_index)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto menu = std::dynamic_pointer_cast<MenuControl>(control)) {
			auto options = configPageCache.GetMenuOptions(menu.get());
			if (a_index >= 0 && a_index < options.size() && menu->ValueSource) {
				menu->ValueSource->SetValue(options[a_index]);
			}

			menu->Refresh(object, a_option);

			SendSettingChangeEvent(a_vm, object, menu->ID);

			menu->InvokeAction(a_vm);
		}
		else if (auto menuEnum = std::dynamic_pointer_cast<EnumControl>(control)) {
			if (menuEnum->ValueSource) {
				menuEnum->ValueSource->SetValue(static_cast<float>(a_index));
			}

			menuEnum->Refresh(object, a_option);

			SendSettingChangeEvent(a_vm, object, menuEnum->ID);

			menuEnum->InvokeAction(a_vm);
		}
	}

	void MCM_ConfigBase::OnOptionColorOpen(RE::TESQuest* a_self, std::int32_t a_option)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto color = std::dynamic_pointer_cast<ColorControl>(control)) {
			if (color->ValueSource) {
				auto startValue = static_cast<std::uint32_t>(color->ValueSource->GetValue());
				SkyUI::Config::SetColorDialogStartColor(object, startValue);

				auto defaultValue = static_cast<std::uint32_t>(
					color->ValueSource->GetDefaultValue());
				SkyUI::Config::SetColorDialogDefaultColor(object, defaultValue);
			}
		}
	}

	void MCM_ConfigBase::OnOptionColorAccept(
		RE::BSScript::IVirtualMachine* a_vm,
		[[maybe_unused]] RE::VMStackID a_stackID,
		RE::TESQuest* a_self,
		std::int32_t a_option,
		std::uint32_t a_color)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto color = std::dynamic_pointer_cast<ColorControl>(control)) {
			if (color->ValueSource) {
				color->ValueSource->SetValue(static_cast<float>(a_color));
			}

			color->Refresh(object, a_option);

			SendSettingChangeEvent(a_vm, object, color->ID);

			color->InvokeAction(a_vm);
		}
	}

	void MCM_ConfigBase::OnOptionKeyMapChange(
		RE::BSScript::IVirtualMachine* a_vm,
		[[maybe_unused]] RE::VMStackID a_stackID,
		RE::TESQuest* a_self,
		std::int32_t a_option,
		std::uint32_t a_keyCode,
		std::string_view a_conflictControl,
		std::string_view a_conflictName)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto keymap = std::dynamic_pointer_cast<KeyMapControl>(control)) {
			std::function<void(bool)> updateKey = [=](bool confirm)
			{
				if (confirm) {
					if (keymap->ValueSource) {
						keymap->ValueSource->SetValue(static_cast<float>(a_keyCode));
					}
					else if (!keymap->ID.empty()) {
						auto modName = FormUtil::GetModName(a_self);
						auto& keybindManager = KeybindManager::GetInstance();
						keybindManager.Register(a_keyCode, modName, keymap->ID);
						keybindManager.CommitKeybinds();
					}

					keymap->Refresh(object, a_option);
					SendSettingChangeEvent(a_vm, object, keymap->ID);
				}
			};

			if (!a_conflictControl.empty() && !keymap->IgnoreConflicts) {
				auto conflictControl = Translation::ScaleformTranslate(
					std::string{ a_conflictControl });

				std::string msg;
				if (!a_conflictName.empty()) {
					msg = fmt::format(
						"$MCM_KeyAlreadyMappedByMod{{{}}}{{{}}}"sv,
						conflictControl,
						a_conflictName);
				}
				else {
					msg = fmt::format("$MCM_KeyAlreadyMapped{{{}}}"sv, conflictControl);
				}

				SkyUI::Config::ShowMessage(object, msg, updateKey);
			}
			else {
				updateKey(true);
			}

			keymap->InvokeAction(a_vm);
		}
	}

	void MCM_ConfigBase::OnOptionInputOpen(RE::TESQuest* a_self, std::int32_t a_option)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto input = std::dynamic_pointer_cast<InputControl>(control)) {
			if (!input->ID.empty()) {
				auto modName = FormUtil::GetModName(a_self);
				auto text = SettingStore::GetInstance().GetModSettingString(modName, input->ID);
				SkyUI::Config::SetInputDialogStartText(object, text);
			}
		}
	}

	void MCM_ConfigBase::OnOptionInputAccept(
		RE::BSScript::IVirtualMachine* a_vm,
		[[maybe_unused]] RE::VMStackID a_stackID,
		RE::TESQuest* a_self,
		std::int32_t a_option,
		std::string_view a_input)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();
		if (a_self != configPageCache.GetCurrentForm())
			return;

		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto control = configPageCache.GetControl(a_option);

		if (auto input = std::dynamic_pointer_cast<InputControl>(control)) {
			if (input->ValueSource) {
				input->ValueSource->SetValue(a_input);
			}

			SendSettingChangeEvent(a_vm, object, input->ID);

			input->InvokeAction(a_vm);
		}
	}

	auto MCM_ConfigBase::GetCustomControl(RE::TESQuest* a_self, std::uint32_t a_keyCode)
		-> std::string
	{
		auto config = ConfigStore::GetInstance().GetConfig(a_self);
		return config ? config->GetCustomControl(a_keyCode) : ""s;
	}

	void MCM_ConfigBase::LoadConfig(RE::TESQuest* a_self)
	{
		auto object = ScriptObject::FromForm(a_self, ScriptName);

		auto modName = FormUtil::GetModName(a_self);

		auto& configStore = ConfigStore::GetInstance();
		if (modName.empty() || configStore.GetConfig(modName)) {
			return;
		}

		auto startTime = std::chrono::steady_clock::now();

		if (!ConfigStore::GetInstance().ReadConfig(modName, object)) {
			return;
		}

		auto modNameVar = object->GetProperty("ModName"sv);
		auto displayName = modNameVar && modNameVar->IsString() ? modNameVar->GetString() : ""sv;

		auto& keybindManager = KeybindManager::GetInstance();
		keybindManager.ReadKeybinds(modName);

		auto endTime = std::chrono::steady_clock::now();
		auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
			endTime - startTime);

		logger::info("Registered mod config for {} in {} ms."sv, modName, elapsedMs.count());
	}

	void MCM_ConfigBase::UpdateInfoText(ScriptObjectPtr a_object, bool a_forceUpdate)
	{
		auto& configPageCache = ConfigPageCache::GetInstance();

		auto control = configPageCache.GetHighlightedControl();
		auto infoText = control ? control->GetInfoText() : ""s;
		if (control && !infoText.empty()) {
			SkyUI::Config::SetInfoText(a_object, infoText, a_forceUpdate);
		}
	}

	void MCM_ConfigBase::SendSettingChangeEvent(
		RE::BSScript::IVirtualMachine* a_vm,
		ScriptObjectPtr a_object,
		std::string a_ID)
	{
		assert(a_vm);

		ScriptCallbackPtr nullCallback;
		auto args = RE::MakeFunctionArguments(std::move(a_ID));
		a_vm->DispatchMethodCall(a_object, "OnSettingChange"sv, args, nullCallback);
		delete args;

		UpdateInfoText(a_object, true);
	}

	bool MCM_ConfigBase::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
	{
		REGISTER_FUNCTION(a_vm, RefreshMenu);
		REGISTER_FUNCTION(a_vm, SetMenuOptions);

		REGISTER_FUNCTION(a_vm, GetModSettingInt);
		REGISTER_FUNCTION(a_vm, GetModSettingBool);
		REGISTER_FUNCTION(a_vm, GetModSettingFloat);
		REGISTER_FUNCTION(a_vm, GetModSettingString);

		REGISTER_FUNCTION(a_vm, SetModSettingInt);
		REGISTER_FUNCTION(a_vm, SetModSettingBool);
		REGISTER_FUNCTION(a_vm, SetModSettingFloat);
		REGISTER_FUNCTION(a_vm, SetModSettingString);

		REGISTER_FUNCTION(a_vm, OnPageReset);

		REGISTER_FUNCTION(a_vm, OnOptionHighlight);
		REGISTER_FUNCTION(a_vm, OnOptionSelect);
		REGISTER_FUNCTION(a_vm, OnOptionDefault);
		REGISTER_FUNCTION(a_vm, OnOptionSliderOpen);
		REGISTER_FUNCTION(a_vm, OnOptionSliderAccept);
		REGISTER_FUNCTION(a_vm, OnOptionMenuOpen);
		REGISTER_FUNCTION(a_vm, OnOptionMenuAccept);
		REGISTER_FUNCTION(a_vm, OnOptionColorOpen);
		REGISTER_FUNCTION(a_vm, OnOptionColorAccept);
		REGISTER_FUNCTION(a_vm, OnOptionKeyMapChange);
		REGISTER_FUNCTION(a_vm, OnOptionInputOpen);
		REGISTER_FUNCTION(a_vm, OnOptionInputAccept);

		REGISTER_FUNCTION(a_vm, GetCustomControl);

		REGISTER_FUNCTION(a_vm, LoadConfig);

		return true;
	}
}
