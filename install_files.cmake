install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/ESP/MCMHelper.esp"
	DESTINATION "."
	COMPONENT ESP
	EXCLUDE_FROM_ALL
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/ESL/MCMHelper.esp"
	DESTINATION "."
	COMPONENT ESL
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/MCMHelper.bsa"
	DESTINATION "."
	COMPONENT BSA
)

install(
	FILES ${TRANSLATION_FILES}
	DESTINATION "Interface/Translations"
	COMPONENT Loose
	EXCLUDE_FROM_ALL
)

install(
	FILES ${Papyrus_OUTPUT}
	DESTINATION "Scripts"
	COMPONENT Loose
	EXCLUDE_FROM_ALL
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/MCM/Settings/readme.txt"
	DESTINATION
		"MCM/Settings"
	COMPONENT Data
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/data/MCM/Config/SkyUI_SE/config.json"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/MCM/Config/SkyUI_SE/settings.ini"
	DESTINATION
		"MCM/Config/SkyUI_SE"
	COMPONENT Data
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public/SKI_ConfigMenu.psc"
	DESTINATION
		"Source/Scripts"
	COMPONENT Data
)

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/Scripts/MCM.pex"
		"${CMAKE_CURRENT_BINARY_DIR}/Scripts/MCM_ConfigBase.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/SDK/Scripts/SKI_ConfigBase.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/SDK/Scripts/SKI_ConfigManager.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/SDK/Scripts/SKI_PlayerLoadGameAlias.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/data/SDK/Scripts/SKI_QuestBase.pex"
	DESTINATION "Scripts"
	COMPONENT SDK
	EXCLUDE_FROM_ALL
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public/MCM.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public/MCM_ConfigBase.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public/SKI_ConfigBase.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public/SKI_QuestBase.psc"
	DESTINATION "Source/Scripts"
	COMPONENT SDK
	EXCLUDE_FROM_ALL
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/fomod/info.xml"
		"${PROJECT_SOURCE_DIR}/data/fomod/ModuleConfig.xml"
	DESTINATION "fomod"
	COMPONENT Fomod
	EXCLUDE_FROM_ALL
)

install(
	FILES
		"${PROJECT_SOURCE_DIR}/data/fomod/images/mcmhelper.jpg"
		"${PROJECT_SOURCE_DIR}/data/fomod/images/skyrim_se.jpg"
		"${PROJECT_SOURCE_DIR}/data/fomod/images/skyrim_vr.jpg"
	DESTINATION "fomod/images"
	COMPONENT Fomod
	EXCLUDE_FROM_ALL
)
