MCM Helper is a framework for simplifying the creation of Mod Configuration Menus for
[SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604).
It is heavily inspired by
[Mod Configuration Menu for Fallout 4 (F4MCM)](https://www.nexusmods.com/fallout4/mods/21497).

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++
* [SKSE64 SDK](https://skse.silverlock.org/)
	* Add the environment variable `SKSE64Path`

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/Exit-9B/MCM-Helper
cd MCM-Helper
git submodule update --init --recursive

# Skyrim SE
cmake --preset vs2022-windows
cmake --build build --config Release

# Skyrim VR
cmake --preset vs2022-windows-vr
cmake --build buildVR --config Release
```
