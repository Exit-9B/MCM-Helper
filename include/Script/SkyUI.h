#pragma once

#include "Script/ScriptObject.h"

#define MENU_ROOT "_root.ConfigPanelFader.configPanel"

namespace SkyUI
{
	using namespace ScriptObject;

	constexpr std::string_view PluginName = "SkyUI_SE.esp"sv;

	// clang-format off
	enum class Flags : std::int32_t
	{
		None      = 0x00,
		Disable   = 0x01,
		Hide      = 0x02,
		WithUnmap = 0x04,
	};

	enum class CursorFillMode : std::int32_t
	{
		LeftToRight = 1,
		TopToBottom = 2,
	};

	enum class State : std::int32_t
	{
		Default = 0,
		Reset   = 1,
		Slider  = 2,
		Menu    = 3,
		Color   = 4,
		Input   = 5,
	};

	enum class OptionType : std::int32_t
	{
		Empty  = 0,
		Header = 1,
		Text   = 2,
		Toggle = 3,
		Slider = 4,
		Menu   = 5,
		Color  = 6,
		KeyMap = 7,
		Input  = 8,
	};
	// clang-format on

	class ConfigManager
	{
	public:
		static auto GetInstance() -> ScriptObjectPtr;

		static void UpdateDisplayName(ScriptObjectPtr a_configManager, ScriptObjectPtr a_config);

		static void UpdateDisplayName_Barzing(
			ScriptObjectPtr a_configManager,
			ScriptObjectPtr a_config,
			std::int32_t a_subPage);
	};

	/* SKI_ConfigBase script functionality reimplemented for efficiency
	 */
	class Config
	{
	public:
		Config() = delete;

		static void ForcePageReset();

		static void SetTitleText(ScriptObjectPtr a_object, std::string_view a_text);

		static void SetInfoText(ScriptObjectPtr a_object, std::string_view a_text);

		static void SetCursorPosition(ScriptObjectPtr a_object, std::int32_t a_position);

		static void SetCursorFillMode(ScriptObjectPtr a_object, CursorFillMode a_fillMode);

		static auto AddEmptyOption(ScriptObjectPtr a_object) -> std::int32_t;

		static auto AddHeaderOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			Flags a_flags = Flags::None) -> std::int32_t;

		static auto AddTextOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			std::string_view a_value,
			Flags a_flags = Flags::None) -> std::int32_t;

		static auto AddToggleOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			bool a_checked,
			Flags a_flags = Flags::None) -> std::int32_t;

		static auto AddSliderOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			float a_value,
			std::string_view a_formatString = "{0}",
			Flags a_flags = Flags::None) -> std::int32_t;

		static auto AddMenuOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			std::string_view a_value,
			Flags a_flags = Flags::None) -> std::int32_t;

		static auto AddColorOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			std::uint32_t a_color,
			Flags a_flags = Flags::None) -> std::int32_t;

		static auto AddKeyMapOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			std::int32_t a_keyCode,
			Flags a_flags = Flags::None) -> std::int32_t;

		static auto AddInputOption(
			ScriptObjectPtr a_object,
			std::string_view a_text,
			std::string_view a_value,
			Flags a_flags = Flags::None) -> std::int32_t;

		static void LoadCustomContent(
			std::string_view a_source,
			float a_x = 0.0f,
			float a_y = 0.0f);

		static void UnloadCustomContent();

		static void SetOptionFlags(
			ScriptObjectPtr a_object,
			std::int32_t a_option,
			Flags a_flags,
			bool a_noUpdate = false);

		static void SetTextOptionValue(
			ScriptObjectPtr a_object,
			std::int32_t a_option,
			std::string_view a_value,
			bool a_noUpdate = false);

		static void SetToggleOptionValue(
			ScriptObjectPtr a_object,
			std::int32_t a_option,
			bool a_checked,
			bool a_noUpdate = false);

		static void SetSliderOptionValue(
			ScriptObjectPtr a_object,
			std::int32_t a_option,
			float a_value,
			std::string_view a_formatString = "{0}"sv,
			bool a_noUpdate = false);

		static void SetMenuOptionValue(
			ScriptObjectPtr a_object,
			std::int32_t a_option,
			std::string_view a_value,
			bool a_noUpdate = false);

		static void SetColorOptionValue(
			ScriptObjectPtr a_object,
			std::int32_t a_option,
			std::uint32_t a_color,
			bool a_noUpdate = false);

		static void SetKeyMapOptionValue(
			ScriptObjectPtr a_object,
			std::int32_t a_option,
			std::int32_t a_keyCode,
			bool a_noUpdate = false);

		static void SetSliderDialogStartValue(ScriptObjectPtr a_object, float a_value);

		static void SetSliderDialogDefaultValue(ScriptObjectPtr a_object, float a_value);

		static void SetSliderDialogRange(
			ScriptObjectPtr a_object,
			float a_minValue,
			float a_maxValue);

		static void SetSliderDialogInterval(ScriptObjectPtr a_object, float a_value);

		static void SetMenuDialogStartIndex(ScriptObjectPtr a_object, std::int32_t a_value);

		static void SetMenuDialogDefaultIndex(ScriptObjectPtr a_object, std::int32_t a_value);

		static void SetMenuDialogOptions(
			ScriptObjectPtr a_object,
			std::span<std::string> a_options);

		static void SetColorDialogStartColor(ScriptObjectPtr a_object, std::uint32_t a_color);

		static void SetColorDialogDefaultColor(ScriptObjectPtr a_object, std::uint32_t a_color);

		static void SetInputDialogStartText(ScriptObjectPtr a_object, std::string_view a_text);

		static void ShowMessage(
			ScriptObjectPtr a_object,
			std::string_view a_message,
			std::function<void(bool)> a_callback);

		static void ShowMessage(
			ScriptObjectPtr a_object,
			std::string_view a_message,
			bool a_withCancel,
			std::function<void(bool)> a_callback);

		static void ShowMessage(
			ScriptObjectPtr a_object,
			std::string_view a_message,
			std::string_view a_acceptLabel,
			std::function<void(bool)> a_callback);

		static void ShowMessage(
			ScriptObjectPtr a_object,
			std::string_view a_message,
			std::string_view a_acceptLabel,
			std::string_view a_cancelLabel,
			std::function<void(bool)> a_callback);

		static void ShowMessage(
			ScriptObjectPtr a_object,
			std::string_view a_message,
			bool a_withCancel,
			std::string_view a_acceptLabel,
			std::string_view a_cancelLabel,
			std::function<void(bool)> a_callback);

	private:
		static void Error(ScriptObjectPtr a_object, std::string_view a_msg);

		static auto AddOption(
			ScriptObjectPtr a_object,
			OptionType a_optionType,
			std::string_view a_text,
			std::string_view a_strValue,
			float a_numValue,
			Flags a_flags) -> std::int32_t;

		static void SetOptionStrValue(
			ScriptObjectPtr a_object,
			std::int32_t a_index,
			std::string_view a_strValue,
			bool a_noUpdate);

		static void SetOptionNumValue(
			ScriptObjectPtr a_object,
			std::int32_t a_index,
			float a_numValue,
			bool a_noUpdate);

		static void SetOptionValues(
			ScriptObjectPtr a_object,
			std::int32_t a_index,
			std::string_view a_strValue,
			float a_numValue,
			bool a_noUpdate);
	};
}

inline static auto operator|(SkyUI::Flags lhs, SkyUI::Flags rhs) -> SkyUI::Flags
{
	return static_cast<SkyUI::Flags>(
		static_cast<std::int32_t>(lhs) | static_cast<std::int32_t>(rhs));
}