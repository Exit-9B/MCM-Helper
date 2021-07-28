@echo off

cmake -B build -S . -DBUILD_SKYRIMVR=OFF --check-stamp-file "build/CMakeFiles/generate.stamp"
cmake --build build --config Release

cmake -B buildVR -S . -DBUILD_SKYRIMVR=ON --check-stamp-file "buildVR/CMakeFiles/generate.stamp"
cmake --build buildVR --config Release

xcopy "dist\Data" "dist\SkyrimSE" /exclude:xcopy_exclude.txt /s /y
xcopy "dist\Data" "dist\SkyrimVR" /exclude:xcopy_exclude.txt /s /y

xcopy "build\Release\%1.dll" "dist\SkyrimSE\SKSE\Plugins\%1.dll*" /y

xcopy "buildVR\Release\%1.dll" "dist\SkyrimVR\SKSE\Plugins\%1.dll*" /y
