#include "Script/SkyUI.h"

#undef GetObject

namespace SkyUI
{
	constexpr auto ErrorOptionTypeMismatch =
		"Option type mismatch. Expected {} option, page \"{}\", index {}"sv;

	ScriptObjectPtr ConfigManager::GetInstance()
	{
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		const auto configManagerInstance = dataHandler
			? dataHandler->LookupForm(0x00000802, SkyUI::PluginName)
			: nullptr;

		return configManagerInstance
			? ScriptObject::FromForm(configManagerInstance, "SKI_ConfigManager"s)
			: nullptr;
	}

	bool ConfigManager::HasConfig(
		const ScriptObjectPtr& a_configManager,
		const ScriptObjectPtr& a_config)
	{
		if (!a_configManager || !a_config) {
			return false;
		}

		const auto configIDVar = ScriptObject::GetVariable(a_config, "_configID"sv);
		if (!configIDVar) {
			return false;
		}

		auto index = configIDVar->GetSInt();

		if (index < 0) {
			return false;
		}

		const auto modConfigsVar = ScriptObject::GetVariable(a_configManager, "_modConfigs"sv);
		const auto modConfigsArray = modConfigsVar && modConfigsVar->IsArray()
			? modConfigsVar->GetArray()
			: nullptr;

		if (!modConfigsArray) {
			return HasConfig_Barzing(a_configManager, a_config, index / 128);
		}

		if (static_cast<std::size_t>(index) >= modConfigsArray->size()) {
			return false;
		}

		return modConfigsArray->data()[index].GetObject() == a_config;
	}

	bool ConfigManager::HasConfig_Barzing(
		const ScriptObjectPtr& a_configManager,
		const ScriptObjectPtr& a_config,
		std::int32_t a_subPage)
	{
		if (!a_configManager || !a_config) {
			return false;
		}

		const auto configIDVar = ScriptObject::GetVariable(a_config, "_configID"sv);
		if (!configIDVar) {
			return false;
		}

		const auto index = configIDVar->GetSInt() % 128;

		const auto modConfigVarName = a_subPage == 0
			? "_MainMenu"s
			: fmt::format("_modConfigsP{}"sv, a_subPage);

		const auto modConfigsVar = ScriptObject::GetVariable(a_configManager, modConfigVarName);
		const auto modConfigsArray = modConfigsVar && modConfigsVar->IsArray()
			? modConfigsVar->GetArray()
			: nullptr;

		if (!modConfigsArray) {
			return false;
		}

		if (static_cast<std::size_t>(index) >= modConfigsArray->size()) {
			return false;
		}

		return modConfigsArray->data()[index].GetObject() == a_config;
	}

	void ConfigManager::UpdateDisplayName(
		const ScriptObjectPtr& a_configManager,
		const ScriptObjectPtr& a_config)
	{
		assert(a_configManager);
		assert(a_config);

		const auto configIDVar = ScriptObject::GetVariable(a_config, "_configID"sv);
		if (!configIDVar) {
			return;
		}

		const auto modNamesVar = ScriptObject::GetVariable(a_configManager, "_modNames"sv);
		const auto modNamesArray = modNamesVar && modNamesVar->IsArray()
			? modNamesVar->GetArray()
			: nullptr;

		if (!modNamesArray) {
			const auto index = configIDVar->GetSInt();
			return UpdateDisplayName_Barzing(a_configManager, a_config, index / 128);
		}

		const auto modNameVar = a_config->GetProperty("ModName"sv);
		if (modNameVar) {
			const auto size = static_cast<std::int32_t>(modNamesArray->size());
			const auto index = configIDVar->GetSInt();

			if (index >= 0 && index < size) {
				auto& registeredName = modNamesVar->GetArray()->data()[index];
				registeredName.SetString(modNameVar->GetString());
			}
		}
	}

	void ConfigManager::UpdateDisplayName_Barzing(
		const ScriptObjectPtr& a_configManager,
		const ScriptObjectPtr& a_config,
		std::int32_t a_subPage)
	{
		assert(a_configManager);
		assert(a_config);

		const auto configIDVar = ScriptObject::GetVariable(a_config, "_configID"sv);
		if (!configIDVar) {
			return;
		}

		const auto modNameVarName = a_subPage == 0
			? "_MainMenuP"s
			: fmt::format("_modNamesP{}"sv, a_subPage);

		const auto modNamesVar = ScriptObject::GetVariable(a_configManager, modNameVarName);
		const auto modNamesArray = modNamesVar && modNamesVar->IsArray()
			? modNamesVar->GetArray()
			: nullptr;

		if (!modNamesArray) {
			return;
		}

		const auto modNameVar = a_config->GetProperty("ModName"sv);
		if (modNameVar) {
			const auto size = static_cast<std::int32_t>(modNamesArray->size());
			const auto index = configIDVar->GetSInt() % 128;

			if (index >= 0 && index < size) {
				auto& registeredName = modNamesVar->GetArray()->data()[index];
				registeredName.SetString(modNameVar->GetString());
			}
		}
	}

	void Config::ForcePageReset()
	{
		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			movie->Invoke(MENU_ROOT ".forcePageReset", nullptr, nullptr, 0);
		}
	}

	void Config::SetTitleText(
		[[maybe_unused]] const ScriptObjectPtr& a_object,
		std::string_view a_text)
	{
		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			const RE::GFxValue args[]{ a_text };
			movie->Invoke(MENU_ROOT ".setTitleText", nullptr, args, 1);
		}
	}

	void Config::SetInfoText(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		bool a_forceUpdate)
	{
		SetString(a_object, "_infoText"sv, a_text);

		if (a_forceUpdate) {
			const auto ui = RE::UI::GetSingleton();
			const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
			const auto movie = menu ? menu->uiMovie : nullptr;
			if (movie) {
				const RE::GFxValue args[]{ a_text };
				movie->Invoke(MENU_ROOT ".setInfoText", nullptr, args, 1);
			}
		}
	}

	void Config::SetCursorPosition(const ScriptObjectPtr& a_object, std::int32_t a_position)
	{
		if (a_position < 128) {
			SetInt(a_object, "_cursorPosition"sv, a_position);
		}
	}

	void Config::SetCursorFillMode(const ScriptObjectPtr& a_object, CursorFillMode a_fillMode)
	{
		if (a_fillMode == CursorFillMode::LeftToRight ||
			a_fillMode == CursorFillMode::TopToBottom) {
			SetInt(a_object, "_cursorFillMode"sv, static_cast<std::int32_t>(a_fillMode));
		}
	}

	std::int32_t Config::AddEmptyOption(const ScriptObjectPtr& a_object)
	{
		return AddOption(a_object, OptionType::Empty, ""sv, ""sv, 0.0f, Flags::None);
	}

	std::int32_t Config::AddHeaderOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		Flags a_flags)
	{
		return AddOption(a_object, OptionType::Header, a_text, ""sv, 0.0f, a_flags);
	}

	std::int32_t Config::AddTextOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		std::string_view a_value,
		Flags a_flags)
	{
		return AddOption(a_object, OptionType::Text, a_text, a_value, 0.0f, a_flags);
	}

	std::int32_t Config::AddToggleOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		bool a_checked,
		Flags a_flags)
	{
		const float numValue = a_checked ? 1.0f : 0.0f;
		return AddOption(a_object, OptionType::Toggle, a_text, ""sv, numValue, a_flags);
	}

	std::int32_t Config::AddSliderOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		float a_value,
		std::string_view a_formatString,
		Flags a_flags)
	{
		return AddOption(a_object, OptionType::Slider, a_text, a_formatString, a_value, a_flags);
	}

	std::int32_t Config::AddMenuOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		std::string_view a_value,
		Flags a_flags)
	{
		return AddOption(a_object, OptionType::Menu, a_text, a_value, 0.0f, a_flags);
	}

	std::int32_t Config::AddColorOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		std::uint32_t a_color,
		Flags a_flags)
	{
		const float numValue = static_cast<float>(a_color);
		return AddOption(a_object, OptionType::Color, a_text, ""sv, numValue, a_flags);
	}

	std::int32_t Config::AddKeyMapOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		std::int32_t a_keyCode,
		Flags a_flags)
	{
		const float numValue = static_cast<float>(a_keyCode);
		return AddOption(a_object, OptionType::KeyMap, a_text, ""sv, numValue, a_flags);
	}

	std::int32_t Config::AddInputOption(
		const ScriptObjectPtr& a_object,
		std::string_view a_text,
		std::string_view a_value,
		Flags a_flags)
	{
		return AddOption(a_object, OptionType::Input, a_text, a_value, 0.0f, a_flags);
	}

	void Config::LoadCustomContent(std::string_view a_source, float a_x, float a_y)
	{
		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			const RE::GFxValue params[]{ a_x, a_y };
			movie->InvokeNoReturn(MENU_ROOT ".setCustomContentParams", params, 2);

			const RE::GFxValue source[]{ a_source };
			movie->InvokeNoReturn(MENU_ROOT ".loadCustomContent", source, 1);
		}
	}

	void Config::UnloadCustomContent()
	{
		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			movie->InvokeNoReturn(MENU_ROOT ".unloadCustomContent", nullptr, 0);
		}
	}

	void Config::SetOptionFlags(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		Flags a_flags,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state"sv) == static_cast<std::int32_t>(State::Reset)) {
			Error(
				a_object,
				"Cannot set option flags while in OnPageReset(). "sv
				"Pass flags to AddOption instead"sv);
			return;
		}

		std::int32_t index = a_option % 0x100;

		std::int32_t oldFlags = 0;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			oldFlags = optionFlagsBuf->data()[index].GetSInt();
		oldFlags %= 0x100;
		oldFlags += static_cast<std::int32_t>(a_flags) * 0x100;

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			RE::GFxValue params[]{ index, a_flags };
			movie->InvokeNoReturn(MENU_ROOT ".setOptionFlags", params, 2);

			if (!a_noUpdate) {
				movie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}

	void Config::SetTextOptionValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		std::string_view a_value,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Text)) {
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			std::string page = ""s;
			if (pageIdx != -1) {
				page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetString();
			}
			Error(a_object, fmt::format(ErrorOptionTypeMismatch, "text"sv, page, index));
			return;
		}

		SetOptionStrValue(a_object, index, a_value, a_noUpdate);
	}

	void Config::SetToggleOptionValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		bool a_checked,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Toggle)) {
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			std::string page = ""s;
			if (pageIdx != -1) {
				page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetString();
			}
			Error(a_object, fmt::format(ErrorOptionTypeMismatch, "toggle"sv, page, index));
			return;
		}

		SetOptionNumValue(a_object, index, a_checked ? 1.0f : 0.0f, a_noUpdate);
	}

	void Config::SetSliderOptionValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		float a_value,
		std::string_view a_formatString,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Slider)) {
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			std::string page = ""s;
			if (pageIdx != -1) {
				page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetString();
			}
			Error(a_object, fmt::format(ErrorOptionTypeMismatch, "slider"sv, page, index));
			return;
		}

		SetOptionValues(a_object, index, a_formatString, a_value, a_noUpdate);
	}

	void Config::SetMenuOptionValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		std::string_view a_value,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Menu)) {
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			std::string page = ""s;
			if (pageIdx != -1) {
				page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetString();
			}
			Error(a_object, fmt::format(ErrorOptionTypeMismatch, "menu"sv, page, index));
			return;
		}

		SetOptionStrValue(a_object, index, a_value, a_noUpdate);
	}

	void Config::SetColorOptionValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		std::uint32_t a_color,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Color)) {
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			std::string page = ""s;
			if (pageIdx != -1) {
				page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetString();
			}
			Error(a_object, fmt::format(ErrorOptionTypeMismatch, "color"sv, page, index));
			return;
		}

		SetOptionNumValue(a_object, index, static_cast<float>(a_color), a_noUpdate);
	}

	void Config::SetKeyMapOptionValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		std::int32_t a_keyCode,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::KeyMap)) {
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			std::string page = ""s;
			if (pageIdx != -1) {
				page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetString();
			}
			Error(a_object, fmt::format(ErrorOptionTypeMismatch, "keymap"sv, page, index));
			return;
		}

		SetOptionNumValue(a_object, index, static_cast<float>(a_keyCode), a_noUpdate);
	}

	void Config::SetInputOptionValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_option,
		std::string_view a_value,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Input)) {
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			std::string page = ""s;
			if (pageIdx != -1) {
				page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetString();
			}
			Error(a_object, fmt::format(ErrorOptionTypeMismatch, "input"sv, page, index));
			return;
		}

		SetOptionStrValue(a_object, index, a_value, a_noUpdate);
	}

	void Config::SetSliderDialogStartValue(const ScriptObjectPtr& a_object, float a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider)) {
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv))
			sliderParams->data()[0].SetFloat(a_value);
	}

	void Config::SetSliderDialogDefaultValue(const ScriptObjectPtr& a_object, float a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider)) {
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv))
			sliderParams->data()[1].SetFloat(a_value);
	}

	void Config::SetSliderDialogRange(
		const ScriptObjectPtr& a_object,
		float a_minValue,
		float a_maxValue)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider)) {
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv)) {
			sliderParams->data()[2].SetFloat(a_minValue);
			sliderParams->data()[3].SetFloat(a_maxValue);
		}
	}

	void Config::SetSliderDialogInterval(const ScriptObjectPtr& a_object, float a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider)) {
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv))
			sliderParams->data()[4].SetFloat(a_value);
	}

	void Config::SetMenuDialogStartIndex(const ScriptObjectPtr& a_object, std::int32_t a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Menu)) {
			Error(a_object, "Cannot set menu dialog params while outside OnOptionMenuOpen()"sv);
			return;
		}

		if (auto menuParams = GetArray(a_object, "_menuParams"sv))
			menuParams->data()[0].SetFloat(static_cast<float>(a_value));
	}

	void Config::SetMenuDialogDefaultIndex(const ScriptObjectPtr& a_object, std::int32_t a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Menu)) {
			Error(a_object, "Cannot set menu dialog params while outside OnOptionMenuOpen()"sv);
			return;
		}

		if (auto menuParams = GetArray(a_object, "_menuParams"sv))
			menuParams->data()[1].SetFloat(static_cast<float>(a_value));
	}

	void Config::SetMenuDialogOptions(
		const ScriptObjectPtr& a_object,
		std::span<const std::string> a_options)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Menu)) {
			Error(a_object, "Cannot set menu dialog params while outside OnOptionMenuOpen()"sv);
			return;
		}

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			std::vector<RE::GFxValue> args;
			args.reserve(a_options.size());
			for (auto& option : a_options) {
				args.push_back(std::string_view(option));
			}

			movie->InvokeNoReturn(
				MENU_ROOT ".setMenuDialogOptions",
				args.data(),
				static_cast<std::uint32_t>(args.size()));
		}
	}

	void Config::SetColorDialogStartColor(const ScriptObjectPtr& a_object, std::uint32_t a_color)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Color)) {
			Error(a_object, "Cannot set color dialog params while outside OnOptionColorOpen()"sv);
			return;
		}

		if (auto colorParams = GetArray(a_object, "_colorParams"sv))
			colorParams->data()[0].SetUInt(a_color);
	}

	void Config::SetColorDialogDefaultColor(const ScriptObjectPtr& a_object, std::uint32_t a_color)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Color)) {
			Error(a_object, "Cannot set color dialog params while outside OnOptionColorOpen()"sv);
			return;
		}

		if (auto colorParams = GetArray(a_object, "_colorParams"sv))
			colorParams->data()[1].SetUInt(a_color);
	}

	void Config::SetInputDialogStartText(const ScriptObjectPtr& a_object, std::string_view a_text)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Input)) {
			Error(a_object, "Cannot set input dialog params while outside OnOptionInputOpen()"sv);
			return;
		}

		SetString(a_object, "_inputStartText"sv, a_text);
	}

	Co::Task<bool> Config::ShowMessage(const ScriptObjectPtr& a_object, std::string_view a_message)
	{
		return ShowMessage(a_object, a_message, true, "$Accept"sv, "$Cancel"sv);
	}

	Co::Task<bool> Config::ShowMessage(
		const ScriptObjectPtr& a_object,
		std::string_view a_message,
		bool a_withCancel)
	{
		return ShowMessage(a_object, a_message, a_withCancel, "$Accept"sv, "$Cancel"sv);
	}

	Co::Task<bool> Config::ShowMessage(
		const ScriptObjectPtr& a_object,
		std::string_view a_message,
		std::string_view a_acceptLabel)
	{
		return ShowMessage(a_object, a_message, false, a_acceptLabel, ""sv);
	}

	Co::Task<bool> Config::ShowMessage(
		const ScriptObjectPtr& a_object,
		std::string_view a_message,
		std::string_view a_acceptLabel,
		std::string_view a_cancelLabel)
	{
		return ShowMessage(a_object, a_message, true, a_acceptLabel, a_cancelLabel);
	}

	Co::Task<bool> Config::ShowMessage(
		const ScriptObjectPtr& a_object,
		std::string_view a_message,
		bool a_withCancel,
		std::string_view a_acceptLabel,
		std::string_view a_cancelLabel)
	{
		if (GetBool(a_object, "_waitForMessage"sv)) {
			Error(a_object, "Called ShowMessage() while another message was already open"sv);
			co_return false;
		}

		const auto skyrimVM = RE::SkyrimVM::GetSingleton();
		const auto vm = skyrimVM ? skyrimVM->impl : nullptr;
		if (!vm)
			co_return false;

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		auto movie = menu ? menu->uiMovie : nullptr;
		if (!movie)
			co_return false;

		SetBool(a_object, "_waitForMessage"sv, true);
		SetBool(a_object, "_messageResult"sv, false);

		RegisterForModEvent(a_object, "SKICP_messageDialogClosed"sv, "OnMessageDialogClose"sv);

		RE::GFxValue params[]{ a_message, a_acceptLabel, a_withCancel ? a_cancelLabel : ""sv };
		movie->Invoke(MENU_ROOT ".showMessageDialog", nullptr, params, 3);

		while (GetBool(a_object, "_waitForMessage"sv)) {
			ScriptArgs args{ RE::MakeFunctionArguments(0.1f) };
			co_await vm->DispatchStaticCall("Utility"sv, "WaitMenuMode"sv, args.get());
		}

		UnregisterForModEvent(a_object, "SKICP_messageDialogClosed"sv);
		co_return GetBool(a_object, "_messageResult"sv);
	}

	void Config::Error(const ScriptObjectPtr& a_object, std::string_view a_msg)
	{
		// SkyUI uses Debug.Trace, but we will just use the SKSE logger
		logger::error("{} ERROR: {}"sv, a_object->GetTypeInfo()->GetName(), a_msg);
	}

	std::int32_t Config::AddOption(
		const ScriptObjectPtr& a_object,
		OptionType a_optionType,
		std::string_view a_text,
		std::string_view a_strValue,
		float a_numValue,
		Flags a_flags)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Reset)) {
			Error(a_object, fmt::format("Cannot add option {} outside of OnPageReset()", a_text));
			return -1;
		}

		std::int32_t pos = GetInt(a_object, "_cursorPosition"sv);
		if (pos == -1) {
			return -1;
		}

		std::int32_t flags = (static_cast<std::int32_t>(a_flags) * 0x100) +
			static_cast<std::int32_t>(a_optionType);

		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			optionFlagsBuf->data()[pos].SetSInt(flags);

		if (auto textBuf = GetArray(a_object, "_textBuf"sv))
			textBuf->data()[pos].SetString(a_text);

		if (auto strValueBuf = GetArray(a_object, "_strValueBuf"sv))
			strValueBuf->data()[pos].SetString(a_strValue);

		if (auto numValueBuf = GetArray(a_object, "_numValueBuf"sv))
			numValueBuf->data()[pos].SetFloat(a_numValue);

		std::int32_t cursorPosition = GetInt(a_object, "_cursorPosition"sv);
		std::int32_t cursorFillMode = GetInt(a_object, "_cursorFillMode"sv);
		cursorPosition += cursorFillMode;
		SetInt(a_object, "_cursorPosition"sv, cursorPosition);
		if (cursorPosition >= 128) {
			SetInt(a_object, "_cursorPosition"sv, -1);
		}

		return pos + GetInt(a_object, "_currentPageNum"sv) * 0x100;
	}

	void Config::SetOptionStrValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_index,
		std::string_view a_strValue,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state"sv) == static_cast<std::int32_t>(State::Reset)) {
			Error(a_object, "Cannot modify option data while in OnPageReset()"sv);
		}

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			movie->SetVariable(MENU_ROOT ".optionCursorIndex", a_index);
			movie->SetVariable(MENU_ROOT ".optionCursor.strValue", a_strValue);
			if (!a_noUpdate) {
				movie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}

	void Config::SetOptionNumValue(
		const ScriptObjectPtr& a_object,
		std::int32_t a_index,
		float a_numValue,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state"sv) == static_cast<std::int32_t>(State::Reset)) {
			Error(a_object, "Cannot modify option data while in OnPageReset()"sv);
		}

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			movie->SetVariable(MENU_ROOT ".optionCursorIndex", a_index);
			movie->SetVariable(MENU_ROOT ".optionCursor.numValue", a_numValue);
			if (!a_noUpdate) {
				movie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}

	void Config::SetOptionValues(
		const ScriptObjectPtr& a_object,
		std::int32_t a_index,
		std::string_view a_strValue,
		float a_numValue,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state"sv) == static_cast<std::int32_t>(State::Reset)) {
			Error(a_object, "Cannot modify option data while in OnPageReset()"sv);
		}

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		const auto movie = menu ? menu->uiMovie : nullptr;
		if (movie) {
			movie->SetVariable(MENU_ROOT ".optionCursorIndex", a_index);
			movie->SetVariable(MENU_ROOT ".optionCursor.strValue", a_strValue);
			movie->SetVariable(MENU_ROOT ".optionCursor.numValue", a_numValue);
			if (!a_noUpdate) {
				movie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}
}
