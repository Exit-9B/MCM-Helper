install(
	FILES
		"$<TARGET_FILE:${PROJECT_NAME}>"
		"$<TARGET_PDB_FILE:${PROJECT_NAME}>"
	DESTINATION "SKSE/Plugins"
)

if(NOT BUILD_SKYRIMVR)
	install(
		FILES
			"${CMAKE_CURRENT_SOURCE_DIR}/dist/SkyrimSE/MCMHelper.esp"
		DESTINATION "."
	)
else()
	install(
		FILES
			"${CMAKE_CURRENT_SOURCE_DIR}/dist/SkyrimVR/MCMHelper.esp"
		DESTINATION "."
	)
endif()

install(
	FILES
		"${CMAKE_CURRENT_BINARY_DIR}/MCMHelper.bsa"
	DESTINATION "."
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/MCM/Settings/readme.txt"
	DESTINATION
		"MCM/Settings"
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/MCM/Config/SkyUI_SE/config.json"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/MCM/Config/SkyUI_SE/settings.ini"
	DESTINATION
		"MCM/Config/SkyUI_SE"
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts/SKI_ConfigMenu.psc"
	DESTINATION
		"Source/Scripts"
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Scripts/MCM.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Scripts/MCM_ConfigBase.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Scripts/SKI_ConfigBase.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Scripts/SKI_ConfigManager.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Scripts/SKI_PlayerLoadGameAlias.pex"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Scripts/SKI_QuestBase.pex"
	DESTINATION "Scripts"
	COMPONENT SDK
	EXCLUDE_FROM_ALL
)

install(
	FILES
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts/MCM.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts/MCM_ConfigBase.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts/SKI_ConfigBase.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts/SKI_QuestBase.psc"
	DESTINATION "Source/Scripts"
	COMPONENT SDK
	EXCLUDE_FROM_ALL
)
