#pragma once

#include "Co/Task.h"
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
		static ScriptObjectPtr GetInstance();

		static bool HasConfig(
			const ScriptObjectPtr& a_configManager,
			const ScriptObjectPtr& a_config);

		static bool HasConfig_Barzing(
			const ScriptObjectPtr& a_configManager,
			const ScriptObjectPtr& a_config,
			std::int32_t a_subPage);

		static void UpdateDisplayName(
			const ScriptObjectPtr& a_configManager,
			const ScriptObjectPtr& a_config);

		static void UpdateDisplayName_Barzing(
			const ScriptObjectPtr& a_configManager,
			const ScriptObjectPtr& a_config,
			std::int32_t a_subPage);
	};

	/**
	 * SKI_ConfigBase script functionality reimplemented for efficiency
	 */
	class Config
	{
	public:
		Config() = delete;

		static void ForcePageReset();

		static void SetTitleText(const ScriptObjectPtr& a_object, std::string_view a_text);

		static void SetInfoText(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			bool a_forceUpdate = false);

		static void SetCursorPosition(const ScriptObjectPtr& a_object, std::int32_t a_position);

		static void SetCursorFillMode(const ScriptObjectPtr& a_object, CursorFillMode a_fillMode);

		static std::int32_t AddEmptyOption(const ScriptObjectPtr& a_object);

		static std::int32_t AddHeaderOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			Flags a_flags = Flags::None);

		static std::int32_t AddTextOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			std::string_view a_value,
			Flags a_flags = Flags::None);

		static std::int32_t AddToggleOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			bool a_checked,
			Flags a_flags = Flags::None);

		static std::int32_t AddSliderOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			float a_value,
			std::string_view a_formatString = "{0}"sv,
			Flags a_flags = Flags::None);

		static std::int32_t AddMenuOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			std::string_view a_value,
			Flags a_flags = Flags::None);

		static std::int32_t AddColorOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			std::uint32_t a_color,
			Flags a_flags = Flags::None);

		static std::int32_t AddKeyMapOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			std::int32_t a_keyCode,
			Flags a_flags = Flags::None);

		static std::int32_t AddInputOption(
			const ScriptObjectPtr& a_object,
			std::string_view a_text,
			std::string_view a_value,
			Flags a_flags = Flags::None);

		static void LoadCustomContent(
			std::string_view a_source,
			float a_x = 0.0f,
			float a_y = 0.0f);

		static void UnloadCustomContent();

		static void SetOptionFlags(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			Flags a_flags,
			bool a_noUpdate = false);

		static void SetTextOptionValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			std::string_view a_value,
			bool a_noUpdate = false);

		static void SetToggleOptionValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			bool a_checked,
			bool a_noUpdate = false);

		static void SetSliderOptionValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			float a_value,
			std::string_view a_formatString = "{0}"sv,
			bool a_noUpdate = false);

		static void SetMenuOptionValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			std::string_view a_value,
			bool a_noUpdate = false);

		static void SetColorOptionValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			std::uint32_t a_color,
			bool a_noUpdate = false);

		static void SetKeyMapOptionValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			std::int32_t a_keyCode,
			bool a_noUpdate = false);

		static void SetInputOptionValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_option,
			std::string_view a_value,
			bool a_noUpdate = false);

		static void SetSliderDialogStartValue(const ScriptObjectPtr& a_object, float a_value);

		static void SetSliderDialogDefaultValue(const ScriptObjectPtr& a_object, float a_value);

		static void SetSliderDialogRange(
			const ScriptObjectPtr& a_object,
			float a_minValue,
			float a_maxValue);

		static void SetSliderDialogInterval(const ScriptObjectPtr& a_object, float a_value);

		static void SetMenuDialogStartIndex(const ScriptObjectPtr& a_object, std::int32_t a_value);

		static void SetMenuDialogDefaultIndex(
			const ScriptObjectPtr& a_object,
			std::int32_t a_value);

		static void SetMenuDialogOptions(
			const ScriptObjectPtr& a_object,
			std::span<const std::string> a_options);

		static void SetColorDialogStartColor(
			const ScriptObjectPtr& a_object,
			std::uint32_t a_color);

		static void SetColorDialogDefaultColor(
			const ScriptObjectPtr& a_object,
			std::uint32_t a_color);

		static void SetInputDialogStartText(
			const ScriptObjectPtr& a_object,
			std::string_view a_text);

		static Co::Task<bool> ShowMessage(
			const ScriptObjectPtr& a_object,
			std::string_view a_message);

		static Co::Task<bool> ShowMessage(
			const ScriptObjectPtr& a_object,
			std::string_view a_message,
			bool a_withCancel);

		static Co::Task<bool> ShowMessage(
			const ScriptObjectPtr& a_object,
			std::string_view a_message,
			std::string_view a_acceptLabel);

		static Co::Task<bool> ShowMessage(
			const ScriptObjectPtr& a_object,
			std::string_view a_message,
			std::string_view a_acceptLabel,
			std::string_view a_cancelLabel);

		static Co::Task<bool> ShowMessage(
			const ScriptObjectPtr& a_object,
			std::string_view a_message,
			bool a_withCancel,
			std::string_view a_acceptLabel,
			std::string_view a_cancelLabel);

	private:
		static void Error(const ScriptObjectPtr& a_object, std::string_view a_msg);

		static std::int32_t AddOption(
			const ScriptObjectPtr& a_object,
			OptionType a_optionType,
			std::string_view a_text,
			std::string_view a_strValue,
			float a_numValue,
			Flags a_flags);

		static void SetOptionStrValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_index,
			std::string_view a_strValue,
			bool a_noUpdate);

		static void SetOptionNumValue(
			const ScriptObjectPtr& a_object,
			std::int32_t a_index,
			float a_numValue,
			bool a_noUpdate);

		static void SetOptionValues(
			const ScriptObjectPtr& a_object,
			std::int32_t a_index,
			std::string_view a_strValue,
			float a_numValue,
			bool a_noUpdate);
	};
}

inline static SkyUI::Flags operator|(SkyUI::Flags lhs, SkyUI::Flags rhs)
{
	return static_cast<SkyUI::Flags>(
		static_cast<std::int32_t>(lhs) | static_cast<std::int32_t>(rhs));
}
