#include "SkyUI.h"

#include "Utils.h"

namespace SkyUI
{
	auto ConfigManager::GetInstance() -> ScriptObjectPtr
	{
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		auto configManagerInstance = dataHandler->LookupForm(0x00000802, SkyUI::PluginName);
		return
			configManagerInstance
			? Utils::GetScriptObject(configManagerInstance, "SKI_ConfigManager")
			: nullptr;
	}

	void ConfigManager::UpdateDisplayName(ScriptObjectPtr a_configManager, ScriptObjectPtr a_config)
	{
		assert(a_configManager);
		assert(a_config);

		auto configIDVar = Utils::GetVariable(a_config, "_configID"sv);
		auto modNamesVar = Utils::GetVariable(a_configManager, "_modNames"sv);
		auto modNameVar = a_config->GetProperty("ModName"sv);
		if (configIDVar && modNamesVar && modNameVar)
		{
			auto index = configIDVar->GetSInt();
			if (index >= 0)
			{
				auto& registeredName = modNamesVar->GetArray()->data()[index];
				registeredName.SetString(modNameVar->GetString());
			}
		}
	}

	void Config::ForcePageReset()
	{
		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		auto movie = menu ? menu->uiMovie : nullptr;
		if (movie)
		{
			movie->Invoke(MENU_ROOT ".forcePageReset", nullptr, nullptr, 0);
		}
	}

	void Config::SetTitleText(ScriptObjectPtr a_object, std::string_view a_text)
	{
		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		auto movie = menu ? menu->uiMovie : nullptr;
		if (movie)
		{
			RE::GFxValue args[]{ a_text };
			movie->Invoke(MENU_ROOT ".setTitleText", nullptr, args, 1);
		}
	}

	void Config::SetInfoText(ScriptObjectPtr a_object, std::string_view a_text)
	{
		SetString(a_object, "_infoText"sv, a_text);
	}

	void Config::SetCursorPosition(ScriptObjectPtr a_object, std::int32_t a_position)
	{
		if (a_position < 128)
		{
			SetInt(a_object, "_cursorPosition"sv, a_position);
		}
	}

	void Config::SetCursorFillMode(ScriptObjectPtr a_object, CursorFillMode a_fillMode)
	{
		if (a_fillMode == CursorFillMode::LeftToRight ||
			a_fillMode == CursorFillMode::TopToBottom)
		{
			SetInt(a_object, "_cursorFillMode"sv, static_cast<std::int32_t>(a_fillMode));
		}
	}

	auto Config::AddEmptyOption(ScriptObjectPtr a_object) -> std::int32_t
	{
		return AddOption(a_object, OptionType::Empty, ""sv, ""sv, 0.0f, Flags::None);
	}

	auto Config::AddHeaderOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		Flags a_flags)
		-> std::int32_t
	{
		return AddOption(a_object, OptionType::Header, a_text, ""sv, 0.0f, a_flags);
	}

	auto Config::AddTextOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		std::string_view a_value,
		Flags a_flags)
		-> std::int32_t
	{
		return AddOption(a_object, OptionType::Text, a_text, a_value, 0.0f, a_flags);
	}

	auto Config::AddToggleOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		bool a_checked,
		Flags a_flags)
		-> std::int32_t
	{
		float numValue = a_checked ? 1.0f : 0.0f;
		return AddOption(a_object, OptionType::Toggle, a_text, ""sv, numValue, a_flags);
	}

	auto Config::AddSliderOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		float a_value,
		std::string_view a_formatString,
		Flags a_flags)
		-> std::int32_t
	{
		return AddOption(a_object, OptionType::Slider, a_text, a_formatString, a_value, a_flags);
	}

	auto Config::AddMenuOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		std::string_view a_value,
		Flags a_flags)
		-> std::int32_t
	{
		return AddOption(a_object, OptionType::Menu, a_text, a_value, 0.0f, a_flags);
	}

	auto Config::AddColorOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		std::uint32_t a_color,
		Flags a_flags)
		-> std::int32_t
	{
		float numValue = static_cast<float>(a_color);
		return AddOption(a_object, OptionType::Color, a_text, ""sv, numValue, a_flags);
	}

	auto Config::AddKeyMapOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		std::int32_t a_keyCode,
		Flags a_flags)
		-> std::int32_t
	{
		float numValue = static_cast<float>(a_keyCode);
		return AddOption(a_object, OptionType::KeyMap, a_text, ""sv, numValue, a_flags);
	}

	auto Config::AddInputOption(
		ScriptObjectPtr a_object,
		std::string_view a_text,
		std::string_view a_value,
		Flags a_flags)
		-> std::int32_t
	{
		return AddOption(a_object, OptionType::Input, a_text, a_value, 0.0f, a_flags);
	}

	void Config::LoadCustomContent(
		std::string_view a_source,
		float a_x,
		float a_y)
	{
		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		if (menu)
		{
			RE::GFxValue params[]{ a_x, a_y };
			menu->uiMovie->InvokeNoReturn(
				MENU_ROOT ".setCustomContentParams",
				params, 2);

			RE::GFxValue source[]{ a_source };
			menu->uiMovie->InvokeNoReturn(
				MENU_ROOT ".loadCustomContent",
				source, 1);
		}
	}

	void Config::UnloadCustomContent()
	{
		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		if (menu)
		{
			menu->uiMovie->InvokeNoReturn(
				MENU_ROOT ".unloadCustomContent",
				nullptr, 0);
		}
	}

	void Config::SetOptionFlags(
		ScriptObjectPtr a_object,
		std::int32_t a_option,
		Flags a_flags,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state"sv) == static_cast<std::int32_t>(State::Reset))
		{
			Error(
				a_object,
				"Cannot set option flags while in OnPageReset(). "sv
				"Pass flags to AddOption instead"sv);
			return;
		}

		std::int32_t index = a_option % 0x100;

		std::int32_t oldFlags = 0;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"))
			oldFlags = optionFlagsBuf->data()[index].GetSInt();
		oldFlags %= 0x100;
		oldFlags += static_cast<std::int32_t>(a_flags) * 0x100;

		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		if (menu)
		{
			RE::GFxValue params[]{ index, a_flags };
			menu->uiMovie->InvokeNoReturn(MENU_ROOT ".setOptionFlags", params, 2);

			if (!a_noUpdate)
			{
				menu->uiMovie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}

	void Config::SetTextOptionValue(
		ScriptObjectPtr a_object,
		std::int32_t a_option,
		std::string_view a_value,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Text))
		{
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			if (pageIdx != -1)
			{
				std::ostringstream error;
				auto page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetSInt();
				error << "Option type mismatch. Expected text option, page \"" << page
					<< "\", index " << index;
				Error(a_object, error.str());
			}
			else
			{
				std::ostringstream error;
				error << "Option type mismatch. Expected text option, page \"\", index " << index;
				Error(a_object, error.str());
			}
			return;
		}

		SetOptionStrValue(a_object, index, a_value, a_noUpdate);
	}

	void Config::SetToggleOptionValue(
		ScriptObjectPtr a_object,
		std::int32_t a_option,
		bool a_checked,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Toggle))
		{
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			if (pageIdx != -1)
			{
				std::ostringstream error;
				auto page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetSInt();
				error << "Option type mismatch. Expected toggle option, page \"" << page
					<< "\", index " << index;
				Error(a_object, error.str());
			}
			else
			{
				std::ostringstream error;
				error << "Option type mismatch. Expected toggle option, page \"\", index " << index;
				Error(a_object, error.str());
			}
			return;
		}

		SetOptionNumValue(a_object, index, a_checked ? 1.0f : 0.0f, a_noUpdate);
	}

	void Config::SetSliderOptionValue(
		ScriptObjectPtr a_object,
		std::int32_t a_option,
		float a_value,
		std::string_view a_formatString,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Slider))
		{
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			if (pageIdx != -1)
			{
				std::ostringstream error;
				auto page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetSInt();
				error << "Option type mismatch. Expected slider option, page \"" << page
					<< "\", index " << index;
				Error(a_object, error.str());
			}
			else
			{
				std::ostringstream error;
				error << "Option type mismatch. Expected slider option, page \"\", index " << index;
				Error(a_object, error.str());
			}
			return;
		}

		SetOptionValues(a_object, index, a_formatString, a_value, a_noUpdate);
	}

	void Config::SetMenuOptionValue(
		ScriptObjectPtr a_object,
		std::int32_t a_option,
		std::string_view a_value,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Menu))
		{
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			if (pageIdx != -1)
			{
				std::ostringstream error;
				auto page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetSInt();
				error << "Option type mismatch. Expected menu option, page \"" << page
					<< "\", index " << index;
				Error(a_object, error.str());
			}
			else
			{
				std::ostringstream error;
				error << "Option type mismatch. Expected menu option, page \"\", index " << index;
				Error(a_object, error.str());
			}
			return;
		}

		SetOptionStrValue(a_object, index, a_value, a_noUpdate);
	}

	void Config::SetColorOptionValue(
		ScriptObjectPtr a_object,
		std::int32_t a_option,
		std::uint32_t a_color,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::Color))
		{
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			if (pageIdx != -1)
			{
				std::ostringstream error;
				auto page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetSInt();
				error << "Option type mismatch. Expected color option, page \"" << page
					<< "\", index " << index;
				Error(a_object, error.str());
			}
			else
			{
				std::ostringstream error;
				error << "Option type mismatch. Expected color option, page \"\", index " << index;
				Error(a_object, error.str());
			}
			return;
		}

		SetOptionNumValue(a_object, index, static_cast<float>(a_color), a_noUpdate);
	}

	void Config::SetKeyMapOptionValue(
		ScriptObjectPtr a_object,
		std::int32_t a_option,
		std::int32_t a_keyCode,
		bool a_noUpdate)
	{
		std::int32_t index = a_option % 0x100;
		std::int32_t type = -1;
		if (auto optionFlagsBuf = GetArray(a_object, "_optionFlagsBuf"sv))
			type = optionFlagsBuf->data()[index].GetSInt() % 0x100;

		if (type != static_cast<std::int32_t>(OptionType::KeyMap))
		{
			std::int32_t pageIdx = (a_option / 0x100) - 1;
			if (pageIdx != -1)
			{
				std::ostringstream error;
				auto page = a_object->GetProperty("Pages")->GetArray()->data()[pageIdx].GetSInt();
				error << "Option type mismatch. Expected keymap option, page \"" << page
					<< "\", index " << index;
				Error(a_object, error.str());
			}
			else
			{
				std::ostringstream error;
				error << "Option type mismatch. Expected keymap option, page \"\", index " << index;
				Error(a_object, error.str());
			}
			return;
		}

		SetOptionNumValue(a_object, index, static_cast<float>(a_keyCode), a_noUpdate);
	}

	void Config::SetSliderDialogStartValue(ScriptObjectPtr a_object, float a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider))
		{
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv))
			sliderParams->data()[0].SetFloat(a_value);
	}

	void Config::SetSliderDialogDefaultValue(ScriptObjectPtr a_object, float a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider))
		{
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv))
			sliderParams->data()[1].SetFloat(a_value);
	}

	void Config::SetSliderDialogRange(ScriptObjectPtr a_object, float a_minValue, float a_maxValue)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider))
		{
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv))
		{
			sliderParams->data()[2].SetFloat(a_minValue);
			sliderParams->data()[3].SetFloat(a_maxValue);
		}
	}

	void Config::SetSliderDialogInterval(ScriptObjectPtr a_object, float a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Slider))
		{
			Error(
				a_object,
				"Cannot set slider dialog params while outside OnOptionSliderOpen()"sv);
			return;
		}

		if (auto sliderParams = GetArray(a_object, "_sliderParams"sv))
			sliderParams->data()[4].SetFloat(a_value);
	}

	void Config::SetMenuDialogStartIndex(ScriptObjectPtr a_object, std::int32_t a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Menu))
		{
			Error(
				a_object,
				"Cannot set menu dialog params while outside OnOptionMenuOpen()"sv);
			return;
		}

		if (auto menuParams = GetArray(a_object, "_menuParams"sv))
			menuParams->data()[0].SetFloat(static_cast<float>(a_value));
	}

	void Config::SetMenuDialogDefaultIndex(ScriptObjectPtr a_object, std::int32_t a_value)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Menu))
		{
			Error(
				a_object,
				"Cannot set menu dialog params while outside OnOptionMenuOpen()"sv);
			return;
		}

		if (auto menuParams = GetArray(a_object, "_menuParams"sv))
			menuParams->data()[1].SetFloat(static_cast<float>(a_value));
	}

	void Config::SetMenuDialogOptions(
		ScriptObjectPtr a_object,
		std::span<std::string> a_options)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Menu))
		{
			Error(
				a_object,
				"Cannot set menu dialog params while outside OnOptionMenuOpen()"sv);
			return;
		}

		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		if (menu)
		{
			std::vector<RE::GFxValue> args;
			args.reserve(a_options.size());
			for (auto& option : a_options)
			{
				args.push_back(std::string_view{ option });
			}

			menu->uiMovie->InvokeNoReturn(
				MENU_ROOT ".setMenuDialogOptions",
				args.data(),
				static_cast<std::uint32_t>(args.size()));
		}
	}

	void Config::SetColorDialogStartColor(ScriptObjectPtr a_object, std::uint32_t a_color)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Color))
		{
			Error(
				a_object,
				"Cannot set color dialog params while outside OnOptionColorOpen()"sv);
			return;
		}

		if (auto colorParams = GetArray(a_object, "_colorParams"sv))
			colorParams->data()[0].SetUInt(a_color);
	}

	void Config::SetColorDialogDefaultColor(ScriptObjectPtr a_object, std::uint32_t a_color)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Color))
		{
			Error(
				a_object,
				"Cannot set color dialog params while outside OnOptionColorOpen()"sv);
			return;
		}

		if (auto colorParams = GetArray(a_object, "_colorParams"sv))
			colorParams->data()[1].SetUInt(a_color);
	}

	void Config::SetInputDialogStartText(ScriptObjectPtr a_object, std::string_view a_text)
	{
		if (GetInt(a_object, "_state"sv) != static_cast<std::int32_t>(State::Input))
		{
			Error(
				a_object,
				"Cannot set input dialog params while outside OnOptionInputOpen()"sv);
			return;
		}

		SetString(a_object, "_inputStartText"sv, a_text);
	}

	void Config::ShowMessage(
		ScriptObjectPtr a_object,
		std::string_view a_message,
		std::function<void(bool)> a_callback)
	{
		ShowMessage(a_object, a_message, true, "$Accept"sv, "$Cancel"sv, a_callback);
	}

	void Config::ShowMessage(
		ScriptObjectPtr a_object,
		std::string_view a_message,
		bool a_withCancel,
		std::function<void(bool)> a_callback)
	{
		ShowMessage(a_object, a_message, a_withCancel, "$Accept"sv, "$Cancel"sv, a_callback);
	}

	void Config::ShowMessage(
		ScriptObjectPtr a_object,
		std::string_view a_message,
		std::string_view a_acceptLabel,
		std::function<void(bool)> a_callback)
	{
		ShowMessage(a_object, a_message, false, a_acceptLabel, ""sv, a_callback);
	}

	void Config::ShowMessage(
		ScriptObjectPtr a_object,
		std::string_view a_message,
		std::string_view a_acceptLabel,
		std::string_view a_cancelLabel,
		std::function<void(bool)> a_callback)
	{
		ShowMessage(a_object, a_message, true, a_acceptLabel, a_cancelLabel, a_callback);
	}

	void Config::ShowMessage(
		ScriptObjectPtr a_object,
		std::string_view a_message,
		bool a_withCancel,
		std::string_view a_acceptLabel,
		std::string_view a_cancelLabel,
		std::function<void(bool)> a_callback)
	{
		if (GetBool(a_object, "_waitForMessage"sv))
		{
			Error(a_object, "Called ShowMessage() while another message was already open"sv);
			if (a_callback)
				return a_callback(false);
			return;
		}

		SetBool(a_object, "_waitForMessage"sv, true);
		SetBool(a_object, "_messageResult"sv, false);

		auto skyrimVM = RE::SkyrimVM::GetSingleton();
		auto vm = skyrimVM ? skyrimVM->impl : nullptr;

		const auto ui = RE::UI::GetSingleton();
		const auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		auto movie = menu ? menu->uiMovie : nullptr;
		if (vm && movie)
		{
			RegisterForModEvent(a_object, "SKICP_messageDialogClosed"sv, "OnMessageDialogClose"sv);

			RE::GFxValue params[]{ a_message, a_acceptLabel, a_withCancel ? a_cancelLabel : ""sv };
			movie->Invoke(MENU_ROOT ".showMessageDialog", nullptr, params, 3);

			struct Awaiter : RE::BSScript::IStackCallbackFunctor {
				ScriptObjectPtr self;
				std::function<void(bool)> callback;

				Awaiter(ScriptObjectPtr a_self, std::function<void(bool)> a_callback) :
					self(a_self), callback(a_callback)
				{}

				virtual void operator()([[maybe_unused]] RE::BSScript::Variable a_result) override
				{
					auto skyrimVM = RE::SkyrimVM::GetSingleton();
					auto vm = skyrimVM ? skyrimVM->impl : nullptr;

					if (!GetBool(self, "_waitForMessage"sv))
					{
						UnregisterForModEvent(self, "SKICP_messageDialogClosed"sv);
						if (callback)
							return callback(GetBool(self, "_messageResult"sv));
						return;
					}

					auto loopCallback = ScriptCallbackPtr{ new Awaiter{self, callback} };

					if (vm)
					{
						auto args = RE::MakeFunctionArguments(0.1f);
						vm->DispatchStaticCall("Utility", "WaitMenuMode", args, loopCallback);
						delete args;
					}
				}

				virtual void SetObject([[maybe_unused]] const ScriptObjectPtr& a_object) override {}
			};

			auto waitCallback = ScriptCallbackPtr{ new Awaiter{a_object, a_callback} };

			if (vm)
			{
				auto args = RE::MakeFunctionArguments(0.1f);
				vm->DispatchStaticCall("Utility", "WaitMenuMode", args, waitCallback);
				delete args;
			}
		}
	}

	void Config::Error(ScriptObjectPtr a_object, std::string_view a_msg)
	{
		// SkyUI uses Debug.Trace, but we will just use the SKSE logger
		logger::error("{} ERROR: {}"sv, a_object->GetTypeInfo()->GetName(), a_msg);
	}

	auto Config::AddOption(
		ScriptObjectPtr a_object,
		OptionType a_optionType,
		std::string_view a_text,
		std::string_view a_strValue,
		float a_numValue,
		Flags a_flags)
		-> std::int32_t
	{
		if (GetInt(a_object, "_state") != static_cast<std::int32_t>(State::Reset))
		{
			std::ostringstream error;
			error << "Cannot add option " << a_text << " outside of OnPageReset()";
			Error(a_object, error.str());
			return -1;
		}

		std::int32_t pos = GetInt(a_object, "_cursorPosition"sv);
		if (pos == -1)
		{
			return -1;
		}

		std::int32_t flags =
			(static_cast<std::int32_t>(a_flags) * 0x100) +
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
		if (cursorPosition >= 128)
		{
			SetInt(a_object, "_cursorPosition"sv, -1);
		}

		return pos + GetInt(a_object, "_currentPageNum"sv) * 0x100;
	}

	void Config::SetOptionStrValue(
		ScriptObjectPtr a_object,
		std::int32_t a_index,
		std::string_view a_strValue,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state") == static_cast<std::int32_t>(State::Reset))
		{
			Error(a_object, "Cannot modify option data while in OnPageReset()"sv);
		}

		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		if (menu)
		{
			menu->uiMovie->SetVariable(MENU_ROOT ".optionCursorIndex", a_index);
			menu->uiMovie->SetVariable(MENU_ROOT ".optionCursor.strValue", a_strValue);
			if (!a_noUpdate)
			{
				menu->uiMovie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}

	void Config::SetOptionNumValue(
		ScriptObjectPtr a_object,
		std::int32_t a_index,
		float a_numValue,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state") == static_cast<std::int32_t>(State::Reset))
		{
			Error(a_object, "Cannot modify option data while in OnPageReset()"sv);
		}

		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		if (menu)
		{
			menu->uiMovie->SetVariable(MENU_ROOT ".optionCursorIndex", a_index);
			menu->uiMovie->SetVariable(MENU_ROOT ".optionCursor.numValue", a_numValue);
			if (!a_noUpdate)
			{
				menu->uiMovie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}

	void Config::SetOptionValues(
		ScriptObjectPtr a_object,
		std::int32_t a_index,
		std::string_view a_strValue,
		float a_numValue,
		bool a_noUpdate)
	{
		if (GetInt(a_object, "_state") == static_cast<std::int32_t>(State::Reset))
		{
			Error(a_object, "Cannot modify option data while in OnPageReset()"sv);
		}

		auto ui = RE::UI::GetSingleton();
		auto menu = ui ? ui->GetMenu<RE::JournalMenu>() : nullptr;
		if (menu)
		{
			menu->uiMovie->SetVariable(MENU_ROOT ".optionCursorIndex", a_index);
			menu->uiMovie->SetVariable(MENU_ROOT ".optionCursor.strValue", a_strValue);
			menu->uiMovie->SetVariable(MENU_ROOT ".optionCursor.numValue", a_numValue);
			if (!a_noUpdate)
			{
				menu->uiMovie->InvokeNoReturn(MENU_ROOT ".invalidateOptionData", nullptr, 0);
			}
		}
	}
}