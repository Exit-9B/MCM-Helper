@echo off

cmake -B build -S . -DBUILD_SKYRIMVR=OFF --check-stamp-file "build/CMakeFiles/generate.stamp"
if %ERRORLEVEL% NEQ 0 exit 1
cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 exit 1

cmake -B buildVR -S . -DBUILD_SKYRIMVR=ON --check-stamp-file "buildVR/CMakeFiles/generate.stamp"
if %ERRORLEVEL% NEQ 0 exit 1
cmake --build buildVR --config Release
if %ERRORLEVEL% NEQ 0 exit 1

xcopy "dist\Data" "%2\SkyrimSE" /i /s /y
xcopy "dist\Data" "%2\SkyrimVR" /i /s /y
xcopy "dist\SkyrimSE" "%2\SkyrimSE" /s /y
xcopy "dist\SkyrimVR" "%2\SkyrimVR" /s /y

xcopy "build\Release\%1.dll" "%2\SkyrimSE\SKSE\Plugins\%1.dll*" /y
xcopy "buildVR\Release\%1.dll" "%2\SkyrimVR\SKSE\Plugins\%1.dll*" /y
