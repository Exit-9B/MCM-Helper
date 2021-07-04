MCM Helper is a framework for simplifying the creation of Mod Configuration Menus for
[SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604).
It is heavily inspired by
[Mod Configuration Menu for Fallout 4 (F4MCM)](https://www.nexusmods.com/fallout4/mods/21497).

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [PowerShell](https://github.com/PowerShell/PowerShell/releases/latest)
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2019](https://visualstudio.microsoft.com/)
	* C++ Clang tools for Windows
	* Desktop development with C++

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/Exit-9B/MCM-Helper
cd MCM-Helper
git submodule update --init --recursive
cmake -B build -S .
```
