cmake_minimum_required(VERSION 3.24)

project(
	MCMHelper
	VERSION 1.5.0
	LANGUAGES CXX
)

# MCM Helper public release number
set(PLUGIN_VERSION 14)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/tools/BethesdaCMakeModules/Modules"
	"${PROJECT_SOURCE_DIR}/tools/SKSE-CMakeModules/Modules"
)
include(SKSEPlugin)
include(Papyrus)
include(BSArchive)

set(SKSE_SUPPORT_VR ON)

SKSEPlugin_Add(
	${PROJECT_NAME}
	SOURCE_DIR src
	SOURCES
		.clang-format
		.editorconfig
		README.md
		vcpkg.json
	PRECOMPILE_HEADERS
		src/PCH/PCH.h
)

target_compile_definitions(
	${PROJECT_NAME}
	PRIVATE
		MCM_VERSION_RELEASE=${PLUGIN_VERSION}
)

find_package(tsl-ordered-map CONFIG REQUIRED)
find_package(RapidJSON CONFIG REQUIRED)

find_path(SIMPLEINI_INCLUDE_DIRS "SimpleIni.h")

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		rapidjson
		tsl::ordered_map
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${SIMPLEINI_INCLUDE_DIRS}
)

Papyrus_Add(
	"Papyrus"
	GAME ${SkyrimSE_PATH}
	IMPORTS
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/private"
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public"
		"$ENV{SKSE64Path}/Scripts/Source"
	SOURCES
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public/MCM.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/public/SKI_ConfigMenu.psc"
		"${CMAKE_CURRENT_SOURCE_DIR}/scripts/private/MCM_ConfigBase.psc"
	OPTIMIZE
	ANONYMIZE
)

set(TRANSLATION_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_CZECH.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_ENGLISH.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_FRENCH.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_GERMAN.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_ITALIAN.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_JAPANESE.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_POLISH.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_RUSSIAN.txt"
	"${CMAKE_CURRENT_SOURCE_DIR}/data/Interface/Translations/MCMHelper_SPANISH.txt"
)

BSArchive_Add(
	"BSA"
	OUTPUT "MCMHelper.bsa"
	FORMAT SSE
	FILES
		${Papyrus_OUTPUT}
		${TRANSLATION_FILES}
	PREFIX
		"${CMAKE_CURRENT_SOURCE_DIR}/data"
)

add_dependencies("BSA" "Papyrus")

include(install_files.cmake)
