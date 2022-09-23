cmake_minimum_required(VERSION 3.22)

project(
	MCMHelper
	VERSION 1.3.3
	LANGUAGES CXX
)

set(PLUGIN_VERSION 11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
	message(
		FATAL_ERROR
			"In-source builds are not allowed."
	)
endif()

option(BUILD_SKYRIMVR "Build the mod for Skyrim VR" OFF)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	option(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD "Include INSTALL target to default build." OFF)
endif()

get_filename_component(
	Skyrim64Path
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Bethesda Softworks\\Skyrim Special Edition;installed path]"
	ABSOLUTE CACHE
)

get_filename_component(
	SkyrimVRPath
	"[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Bethesda Softworks\\Skyrim VR;installed path]"
	ABSOLUTE CACHE
)

if(BUILD_SKYRIMVR)
	add_compile_definitions(SKYRIMVR)
	set(GAME_DIR ${SkyrimVRPath})
else()
	set(GAME_DIR ${Skyrim64Path})
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND DEFINED GAME_DIR)
	set(CMAKE_INSTALL_PREFIX "${GAME_DIR}/Data" CACHE PATH
		"Install path prefix (e.g. Skyrim Data directory or Mod Organizer virtual directory)."
		FORCE
	)
endif()

list(APPEND CMAKE_IGNORE_PATH "${PROJECT_BINARY_DIR}")
list(APPEND CMAKE_MODULE_PATH
	"${PROJECT_SOURCE_DIR}/cmake"
	"${PROJECT_SOURCE_DIR}/tools/BethesdaCMakeModules/Modules"
)

include(AddCXXFiles)
include(BSArchive)
include(Papyrus)

add_library("${PROJECT_NAME}" SHARED)

target_compile_features(
	${PROJECT_NAME}
	PRIVATE
		cxx_std_20
)

add_cxx_files("${PROJECT_NAME}")

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/src/Version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/src/Version.h
	@ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

target_sources(
	"${PROJECT_NAME}"
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/src/Version.h
		${CMAKE_CURRENT_BINARY_DIR}/version.rc
		.clang-format
		.editorconfig
		README.md
		vcpkg.json
)

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		src/PCH/PCH.h
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/src
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(
		${PROJECT_NAME}
		PRIVATE
			"/sdl"	# Enable Additional Security Checks
			"/utf-8"	# Set Source and Executable character sets to UTF-8
			"/Zi"	# Debug Information Format

			"/permissive-"	# Standards conformance
			"/Zc:preprocessor"	# Enable preprocessor conformance mode

			"/wd4200" # nonstandard extension used : zero-sized array in struct/union

			"$<$<CONFIG:DEBUG>:>"
			"$<$<CONFIG:RELEASE>:/Zc:inline;/JMC-;/Ob3>"
	)

	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF;/DEBUG:FULL>"
	)
endif()

add_subdirectory("extern/CommonLibSSE" CommonLibSSE EXCLUDE_FROM_ALL)

find_package(spdlog REQUIRED)
find_package(tsl-ordered-map CONFIG REQUIRED)
find_package(RapidJSON REQUIRED)

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
		CommonLibSSE::CommonLibSSE
		spdlog::spdlog
)

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${SIMPLEINI_INCLUDE_DIRS}
)

file(GLOB Papyrus_SOURCES
	LIST_DIRECTORIES false
	CONFIGURE_DEPENDS
	"dist/Detail/Source/Scripts/*.psc"
)

list(APPEND Papyrus_SOURCES
	"dist/Data/Source/Scripts/MCM.psc"
	"dist/Data/Source/Scripts/SKI_ConfigMenu.psc"
)

set(Papyrus_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts/MCM.psc"
	"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts/SKI_ConfigMenu.psc"
	"${CMAKE_CURRENT_SOURCE_DIR}/dist/Detail/Source/Scripts/MCM_ConfigBase.psc"
)

add_papyrus(
	"Papyrus"
	GAME ${Skyrim64Path}
	IMPORTS
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Detail/Source/Scripts"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Source/Scripts"
		"$ENV{SKSE64Path}/Scripts/Source"
	SOURCES ${Papyrus_SOURCES}
	OPTIMIZE
	ANONYMIZE
)

source_group("Scripts" FILES ${Papyrus_SOURCES})

bethesda_archive(
	"BSA"
	OUTPUT "MCMHelper.bsa"
	FORMAT SSE
	FILES
		${Papyrus_OUTPUT}
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_CZECH.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_ENGLISH.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_FRENCH.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_GERMAN.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_ITALIAN.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_JAPANESE.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_POLISH.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_RUSSIAN.txt"
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data/Interface/Translations/MCMHelper_SPANISH.txt"
	PREFIX
		"${CMAKE_CURRENT_SOURCE_DIR}/dist/Data"
)

add_dependencies("BSA" "Papyrus")

include(install_files.cmake)
