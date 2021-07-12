#include "ConfigStore.h"
#include "SkyUI.h"
#include "Utils.h"
#include "Config.h"
#include <nlohmann/json.hpp>

#undef GetObject
using json = nlohmann::json;

ConfigStore::ConfigStore()
{
	logger::info("ModConfigStore initializing."sv);
}

ConfigStore& ConfigStore::GetInstance()
{
	static ConfigStore instance;
	return instance;
}

void ConfigStore::ReadConfigs()
{
	auto startTime = std::chrono::steady_clock::now();

	auto configManager = SkyUI::ConfigManager::GetInstance();
	if (!configManager)
	{
		// Didn't find SkyUI
		logger::warn("Could not find SkyUI Config Manager."sv);
		return;
	}

	auto modConfigsVar = Utils::GetVariable(configManager, "_modConfigs"sv);
	if (!modConfigsVar || !modConfigsVar->IsObjectArray())
	{
		// Something else went wrong
		logger::warn("Could not find registered mod configs."sv);
		return;
	}

	auto modConfigs = modConfigsVar->GetArray();

	if (!modConfigs)
	{
		logger::warn("SkyUI Config Manager is not ready"sv);
		return;
	}

	for (auto& modConfig : *modConfigs)
	{
		auto configScript = modConfig.GetObject();

		if (configScript)
		{
			try {
				auto& configStore = ConfigStore::GetInstance();
				if (configStore.ReadConfig(configScript))
				{
					SkyUI::ConfigManager::UpdateDisplayName(configManager, configScript);
				}
			}
			catch (const json::exception& e) {
				logger::warn(e.what());
			}
		}
	}

	auto endTime = std::chrono::steady_clock::now();
	auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

	logger::info(
		"Registered {} mod configs in {} ms."sv,
		_configStore.size(),
		elapsedMs.count());
}

bool ConfigStore::ReadConfig(ScriptObjectPtr a_configScript)
{
	assert(a_configScript);

	auto typeID = static_cast<RE::VMTypeID>(RE::TESQuest::FORMTYPE);
	auto quest = static_cast<RE::TESQuest*>(a_configScript->Resolve(typeID));
	auto plugin = Utils::GetModName(quest);

	if (plugin.empty())
		return false;

	std::filesystem::path configPath{ "Data/MCM/Config/"sv };
	auto configLocation = configPath / plugin / "config.json"sv;

	json configDefinition;
	std::ifstream configIn{ configLocation };
	try {
		configIn >> configDefinition;
	}
	catch (const json::parse_error& e) {
		logger::error("Error parsing config for {}"sv, plugin);
		logger::error(e.what());
		return false;
	}

	try {
		auto modName = configDefinition.at("modName").get<std::string>();

		if (modName != plugin)
		{
			logger::warn(
				"Mod config for {} reports mismatched mod name: {}"sv,
				plugin,
				modName);

			return false;
		}
	}
	catch (const json::exception&) {
		return false;
	}

	std::string displayName;
	try {
		configDefinition.at("displayName").get_to(displayName);
	}
	catch (const json::exception&) {
		return false;
	}

	auto modNameVariable = a_configScript->GetProperty("ModName");
	if (modNameVariable)
	{
		modNameVariable->SetString(displayName);
	}

	try {
		auto minMcmVersion = configDefinition.at("minMcmVersion").get<std::int32_t>();
		if (minMcmVersion > PLUGIN_VERSION)
		{
			logger::warn(
				"Mod config for {} requires minimum MCM Helper version: {}"sv,
				plugin,
				minMcmVersion);

			return false;
		}
	}
	catch (const json::exception&) {}

	try {
		auto& pluginRequirements = configDefinition.at("pluginRequirements");
		if (pluginRequirements.is_array())
		{
			for (auto& requirement : pluginRequirements)
			{
				if (!requirement.is_string())
					continue;

				const auto dataHandler = RE::TESDataHandler::GetSingleton();
				if (dataHandler && !dataHandler->LookupModByName(requirement))
				{
					logger::warn(
						"Mod config for {} requires plugin: {}"sv,
						requirement,
						requirement.get<std::string>());

					return false;
				}
			}
		}
	}
	catch (const json::exception&) {}

	auto config = std::make_shared<Config>();

	config->MainPage = ReadPage(plugin, configDefinition);
	try {
		auto& pages = configDefinition.at("pages");
		for (auto& page : pages)
		{
			auto pageDisplayName = page.at("pageDisplayName").get<std::string>();
			config->SubPages[pageDisplayName] = ReadPage(plugin, page);
		}
	}
	catch (const json::exception&) {}

	_configStore[plugin] = config;

	const auto skyrimVM = RE::SkyrimVM::GetSingleton();
	const auto vm = skyrimVM ? skyrimVM->impl : nullptr;
	auto pagesVariable = a_configScript->GetProperty("Pages");

	if (vm && pagesVariable)
	{
		ScriptArrayPtr pagesArray;
		vm->CreateArray(
			RE::BSScript::TypeInfo{ RE::BSScript::TypeInfo::RawType::kString },
			static_cast<std::uint32_t>(config->SubPages.size()),
			pagesArray);

		std::uint32_t index = 0;
		for (auto& [pageDisplayName, pageContent] : config->SubPages)
		{
			pagesArray->data()[index].SetString(pageDisplayName);
			index++;
		}

		pagesVariable->SetArray(pagesArray);
	}

	return true;
}

auto ConfigStore::GetConfig(const std::string& a_modName) -> std::shared_ptr<Config>
{
	auto it = _configStore.find(a_modName);
	if (it != _configStore.end())
	{
		return it->second;
	}
	else
	{
		return nullptr;
	}
}

auto ConfigStore::GetConfig(RE::TESQuest* a_configQuest) -> std::shared_ptr<Config>
{
	auto modName = Utils::GetModName(a_configQuest);
	return GetConfig(modName);
}

auto ConfigStore::ReadPage(const std::string& a_modName, const json& a_page)
	-> std::shared_ptr<PageContent>
{
	try {
		auto& customContent = a_page.at("customContent");
		return ReadCustomContent(customContent);
	}
	catch (const json::exception&) {}

	try {
		auto& content = a_page.at("content");
		auto page = ReadContent(a_modName, content);
		if (page)
		{
			try {
				auto cursorFillMode = a_page.at("cursorFillMode").get<std::string>();
				if (cursorFillMode == "leftToRight") {
					page->CursorFillMode = SkyUI::CursorFillMode::LeftToRight;
				}
				else if (cursorFillMode == "topToBottom") {
					page->CursorFillMode = SkyUI::CursorFillMode::TopToBottom;
				}
			}
			catch (const json::exception&) {}

			return page;
		}
	}
	catch (const json::exception&) {}

	return nullptr;
}

auto ConfigStore::ReadCustomContent(const json& a_customContent) -> std::shared_ptr<CustomContent>
{
	auto content = std::make_shared<CustomContent>();

	try {
		auto source = a_customContent.at("source").get<std::string>();
		content->Source = source;
	}
	catch (const json::exception&) {}

	try {
		auto x = a_customContent.at("x").get<float>();
		content->X = x;
	}
	catch (const json::exception&) {}

	try {
		auto y = a_customContent.at("y").get<float>();
		content->Y = y;
	}
	catch (const json::exception&) {}

	return content;
}

auto ConfigStore::ReadContent(const std::string& a_modName, const json& a_content)
	-> std::shared_ptr<PageLayout>
{
	auto content = std::make_shared<PageLayout>();

	for (auto& element : a_content)
	{
		std::shared_ptr<Control> control;

		std::string id;
		try {
			element.at("id").get_to(id);
		}
		catch (const json::exception&) {}

		try {
			auto type = element.at("type").get<std::string>();
			if (type == "empty") {
				control = std::make_shared<EmptyControl>();
			}
			else if (type == "header") {
				auto headerOption = std::make_shared<HeaderControl>();
				element.at("text").get_to(headerOption->Text);
				control = headerOption;
			}
			else if (type == "text") {
				auto textOption = std::make_shared<TextControl>();
				element.at("text").get_to(textOption->Text);

				try {
					auto& valueOptions = element.at("valueOptions");
					try {
						valueOptions.at("value").get_to(textOption->Value);
					}
					catch (const json::exception&) {}

					try {
						valueOptions.at("propertyName").get_to(textOption->PropertyName);
						try {
							valueOptions.at("scriptName").get_to(textOption->ScriptName);
						}
						catch (const json::exception&) {}
					}
					catch (const json::exception&) {}

					textOption->ModName = a_modName;

					try {
						auto sourceForm = valueOptions.at("sourceForm").get<std::string>();
						textOption->SourceForm = Utils::GetFormFromIdentifier(sourceForm);
					}
					catch (const json::exception&) {}
				}
				catch (const json::exception&) {}
				control = textOption;
			}
			else if (type == "toggle" || type == "hiddenToggle") {
				auto toggleOption =
					type == "hiddenToggle"
					? std::make_shared<HiddenToggleControl>()
					: std::make_shared<ToggleControl>();

				try {
					element.at("text").get_to(toggleOption->Text);
				}
				catch (const json::exception&) {}

				try {
					element.at("groupControl").get_to(toggleOption->GroupControl);

					content->GroupControls[toggleOption->GroupControl] = toggleOption;
				}
				catch (const json::exception&) {}

				try {
					auto& valueOptions = element.at("valueOptions");
					toggleOption->ValueSource = ReadValueSource(a_modName, id, valueOptions);
				}
				catch (const json::out_of_range&) {}

				control = toggleOption;
			}
			else if (type == "slider") {
				auto sliderOption = std::make_shared<SliderControl>();
				try {
					element.at("text").get_to(sliderOption->Text);
				}
				catch (const json::exception&) {}

				auto& valueOptions = element.at("valueOptions");
				valueOptions.at("min").get_to(sliderOption->Min);
				valueOptions.at("max").get_to(sliderOption->Max);
				valueOptions.at("step").get_to(sliderOption->Step);
				sliderOption->ValueSource = ReadValueSource(a_modName, id, valueOptions);

				try {
					element.at("formatString").get_to(sliderOption->FormatString);
				}
				catch (const json::exception&) {}

				control = sliderOption;
			}
			else if (type == "stepper") {
				auto stepperOption = std::make_shared<StepperControl>();
				try {
					element.at("text").get_to(stepperOption->Text);
				}
				catch (const json::exception&) {}

				auto& valueOptions = element.at("valueOptions");
				try {
					auto& options = valueOptions.at("options");
					for (auto& option : options)
					{
						stepperOption->Options.push_back(option.get<std::string>());
					}
				}
				catch (const json::exception&) {}
				stepperOption->ValueSource = ReadValueSource(a_modName, id, valueOptions);
				control = stepperOption;
			}
			else if (type == "menu") {
				auto menuOption = std::make_shared<MenuControl>();
				try {
					element.at("text").get_to(menuOption->Text);
				}
				catch (const json::exception&) {}

				auto& valueOptions = element.at("valueOptions");
				try {
					auto& options = valueOptions.at("options");
					for (auto& option : options)
					{
						menuOption->Options.push_back(option.get<std::string>());
					}
				}
				catch (const json::exception&) {}

				try {
					auto& shortNames = valueOptions.at("shortNames");
					for (auto& shortName : shortNames)
					{
						menuOption->ShortNames.push_back(shortName.get<std::string>());
					}
				}
				catch (const json::exception&) {}

				valueOptions.at("propertyName").get_to(menuOption->PropertyName);
				try {
					valueOptions.at("scriptName").get_to(menuOption->ScriptName);
				}
				catch (const json::exception&) {}

				menuOption->ModName = a_modName;

				try {
					auto sourceForm = valueOptions.at("sourceForm").get<std::string>();
					menuOption->SourceForm = Utils::GetFormFromIdentifier(sourceForm);
				}
				catch (const json::exception&) {}

				control = menuOption;
			}
			else if (type == "enum") {
				auto enumOption = std::make_shared<EnumControl>();
				try {
					element.at("text").get_to(enumOption->Text);
				}
				catch (const json::exception&) {}

				auto& valueOptions = element.at("valueOptions");
				try {
					auto& options = valueOptions.at("options");
					for (auto& option : options)
					{
						enumOption->Options.push_back(option.get<std::string>());
					}
				}
				catch (const json::exception&) {}

				try {
					auto& shortNames = valueOptions.at("shortNames");
					for (auto& shortName : shortNames)
					{
						enumOption->ShortNames.push_back(shortName.get<std::string>());
					}
				}
				catch (const json::exception&) {}

				enumOption->ValueSource = ReadValueSource(a_modName, id, valueOptions);
				control = enumOption;
			}
			else if (type == "color") {
				auto colorOption = std::make_shared<ColorControl>();
				try {
					element.at("text").get_to(colorOption->Text);
				}
				catch (const json::exception&) {}

				try {
					auto& valueOptions = element.at("valueOptions");
					colorOption->ValueSource = ReadValueSource(a_modName, id, valueOptions);
				}
				catch (const json::out_of_range&) {}
				control = colorOption;
			}
			else if (type == "keymap") {
				auto keyMapOption = std::make_shared<KeyMapControl>();
				try {
					element.at("text").get_to(keyMapOption->Text);
				}
				catch (const json::exception&) {}

				try {
					element.at("ignoreConflicts").get_to(keyMapOption->IgnoreConflicts);
				}
				catch (const json::exception&) {}

				try {
					auto& valueOptions = element.at("valueOptions");
					keyMapOption->ValueSource = ReadValueSource(a_modName, id, valueOptions);
				}
				catch (const json::out_of_range&) {}
				control = keyMapOption;
			}
			else if (type == "input") {
				auto inputOption = std::make_shared<InputControl>();
				try {
					element.at("text").get_to(inputOption->Text);
				}
				catch (const json::exception&) {}

				auto& valueOptions = element.at("valueOptions");
				valueOptions.at("propertyName").get_to(inputOption->PropertyName);
				try {
					valueOptions.at("scriptName").get_to(inputOption->ScriptName);
				}
				catch (const json::exception&) {}

				inputOption->ModName = a_modName;

				try {
					auto sourceForm = valueOptions.at("sourceForm").get<std::string>();
					inputOption->SourceForm = Utils::GetFormFromIdentifier(sourceForm);
				}
				catch (const json::exception&) {}

				control = inputOption;
			}
			else {
				logger::warn("Unrecognized control type: {}"sv, type);
				continue;
			}
		}
		catch (const json::exception&) {
			continue;
		}

		assert(control);

		control->ID = id;

		try {
			element.at("position").get_to(control->Position);
		}
		catch (const json::exception&) {}

		try {
			element.at("help").get_to(control->Help);
		}
		catch (const json::exception&) {}

		try {
			auto& action = element.at("action");
			control->Action = ReadAction(action);
		}
		catch (const json::exception&) {}

		try {
			auto& groupCondition = element.at("groupCondition");
			control->GroupCondition = ReadGroupCondition(groupCondition);
			try {
				auto groupBehavior = element.at("groupBehavior").get<std::string>();
				if (groupBehavior == "disable") {
					control->GroupBehavior = Control::Behavior::Disable;
				}
				else if (groupBehavior == "hide") {
					control->GroupBehavior = Control::Behavior::Hide;
				}
				else if (groupBehavior == "skip") {
					control->GroupBehavior = Control::Behavior::Skip;
				}
			}
			catch (const json::exception&) {}
		}
		catch (const json::exception&) {}

		content->Controls.push_back(control);
	}

	return content;
}

auto ConfigStore::ReadValueSource(
	const std::string& a_modName,
	const std::string& a_ID,
	const json& a_valueOptions)
	-> std::shared_ptr<ValueSource>
{
	try {
		auto sourceType = a_valueOptions.at("sourceType").get<std::string>();
		if (sourceType.rfind("PropertyValue", 0) == 0)
		{
			std::shared_ptr<PropertyValue> propertyValue = nullptr;
			if (sourceType == "PropertyValueBool") {
				propertyValue = std::make_shared<PropertyValueBool>();
			}
			else if (sourceType == "PropertyValueInt") {
				propertyValue = std::make_shared<PropertyValueInt>();
			}
			else if (sourceType == "PropertyValueFloat") {
				propertyValue = std::make_shared<PropertyValueFloat>();
			}

			if (propertyValue)
			{
				a_valueOptions.at("propertyName").get_to(propertyValue->PropertyName);
				try {
					a_valueOptions.at("scriptName").get_to(propertyValue->ScriptName);
				}
				catch (const json::exception&) {}

				try {
					auto sourceForm = a_valueOptions.at("sourceForm").get<std::string>();
					propertyValue->SourceForm = Utils::GetFormFromIdentifier(sourceForm);
				}
				catch (const json::exception&) {}

				return propertyValue;
			}
		}
		else if (sourceType.rfind("ModSetting", 0) == 0)
		{
			std::shared_ptr<ModSetting> modSetting = nullptr;
			if (sourceType == "ModSettingBool") {
				modSetting = std::make_shared<ModSettingBool>();
			}
			else if (sourceType == "ModSettingInt") {
				modSetting = std::make_shared<ModSettingInt>();
			}
			else if (sourceType == "ModSettingFloat") {
				modSetting = std::make_shared<ModSettingFloat>();
			}

			if (modSetting)
			{
				modSetting->ModName = a_modName;
				modSetting->ID = a_ID;

				return modSetting;
			}
		}
		else if (sourceType == "GlobalValue")
		{
			auto globalValue = std::make_shared<GlobalValue>();
			auto sourceForm = a_valueOptions.at("sourceForm").get<std::string>();
			auto form = Utils::GetFormFromIdentifier(sourceForm);
			globalValue->SourceForm = skyrim_cast<RE::TESGlobal*>(form);

			return globalValue;
		}

		logger::warn("Source type {} was not recognized."sv, sourceType);
		return nullptr;
	}
	catch (const json::exception&) {
		logger::warn("Failed to parse value source. ({}:{})"sv, a_modName, a_ID);
		return nullptr;
	}
}

auto ConfigStore::ReadGroupCondition(const json& a_groupCondition)
	-> std::shared_ptr<GroupConditionTree>
{
	auto groupCondition = std::make_shared<GroupConditionTree>();

	if (a_groupCondition.is_number_integer())
	{
		auto groupID = a_groupCondition.get<std::uint32_t>();
		groupCondition->TopLevelOperands = { groupID };
		return groupCondition;
	}

	json operands = nullptr;
	if (a_groupCondition.is_array())
	{
		operands = a_groupCondition;
	}
	else if (a_groupCondition.is_object())
	{
		try {
			operands = a_groupCondition.at("OR");
			groupCondition->Conjunction = GroupConditionTree::ConjunctionType::OR;
		}
		catch (const json::exception&) {}

		try {
			operands = a_groupCondition.at("AND");
			groupCondition->Conjunction = GroupConditionTree::ConjunctionType::AND;
		}
		catch (const json::exception&) {}

		try {
			operands = a_groupCondition.at("ONLY");
			groupCondition->Conjunction = GroupConditionTree::ConjunctionType::ONLY;
		}
		catch (const json::exception&) {}

		try {
			operands = a_groupCondition.at("NOT");
			groupCondition->Conjunction = GroupConditionTree::ConjunctionType::NOT;
		}
		catch (const json::exception&) {}

		if (operands.is_number_integer())
		{
			groupCondition->TopLevelOperands.push_back(operands.get<std::uint32_t>());
		}
		else
		{
			if (!operands.is_array())
				return nullptr;

			for (auto& operand : operands)
			{
				if (operand.is_number_integer())
				{
					groupCondition->TopLevelOperands.push_back(operand.get<std::uint32_t>());
				}
				else
				{
					auto subtree = ReadGroupCondition(operand);
					if (!subtree)
						return nullptr;

					groupCondition->SubTrees.push_back(subtree);
				}
			}
		}
	}

	return groupCondition;
}

auto ConfigStore::ReadAction(const json& a_action) -> std::shared_ptr<Action>
{
	auto type = a_action.at("type").get<std::string>();

	std::shared_ptr<Action> action;
	if (type == "CallFunction")
	{
		auto callFunction = std::make_shared<CallFunction>();

		try {
			auto formIdentifier = a_action.at("form").get<std::string>();
			auto form = Utils::GetFormFromIdentifier(formIdentifier);
			callFunction->Form = form;
		}
		catch (const json::exception&) {}

		try {
			a_action.at("scriptName").get_to(callFunction->ScriptName);
		}
		catch (const json::exception&) {}

		action = callFunction;
	}
	else if (type == "CallGlobalFunction")
	{
		auto callGlobalFunction = std::make_shared<CallGlobalFunction>();

		try {
			a_action.at("script").get_to(callGlobalFunction->ScriptName);
		}
		catch (const json::exception&) {}

		action = callGlobalFunction;
	}

	if (action)
	{
		a_action.at("function").get_to(action->Function);

		try {
			auto& params = a_action.at("params");
			for (auto& param : params)
			{
				std::ostringstream ss;
				if (param.is_boolean()) {
					ss << "{b}" << param.get<bool>();
				}
				else if (param.is_number_integer()) {
					ss << "{i}" << param.get<std::int32_t>();
				}
				else if (param.is_number_float()) {
					ss << "{f}" << param.get<float>();
				}
				else if (param.is_string()) {
					ss << param.get<std::string>();
				}
				action->Params.push_back(ss.str());
			}
		}
		catch (const json::exception&) {}
	}

	return action;
}
