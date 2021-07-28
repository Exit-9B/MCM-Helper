@echo off

cmake -B build -S . -DBUILD_SKYRIMVR=OFF
if %ERRORLEVEL% NEQ 0 exit 1
cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 exit 1

cmake -B buildVR -S . -DBUILD_SKYRIMVR=ON
if %ERRORLEVEL% NEQ 0 exit 1
cmake --build buildVR --config Release
if %ERRORLEVEL% NEQ 0 exit 1

xcopy "dist\Data" "dist\SkyrimSE" /exclude:xcopy_exclude.txt /s /y
xcopy "dist\Data" "dist\SkyrimVR" /exclude:xcopy_exclude.txt /s /y

xcopy "build\Release\%1.dll" "dist\SkyrimSE\SKSE\Plugins\%1.dll*" /y

xcopy "buildVR\Release\%1.dll" "dist\SkyrimVR\SKSE\Plugins\%1.dll*" /y
