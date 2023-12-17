@echo off

md "package\Fomod"
md "package\SDK"

cmake --preset vs2022-windows --check-stamp-file "build/CMakeFiles/generate.stamp" -Wno-deprecated || goto :error
cmake --build build --config Release || goto :error
cmake --install build --component "SKSEPlugin" --prefix "package/Fomod/SkyrimSE" || goto :error

cmake --preset vs2022-windows-vr --check-stamp-file "buildVR/CMakeFiles/generate.stamp" -Wno-deprecated || goto :error
cmake --build buildVR --target "MCMHelper" --config Release || goto :error
cmake --install buildVR --component "SKSEPlugin" --prefix "package/Fomod/SkyrimVR" || goto :error

cmake --install build --component "Fomod" --prefix "package/Fomod" || goto :error
cmake --install build --component "ESP" --prefix "package/Fomod/ESP" || goto :error
cmake --install build --component "ESL" --prefix "package/Fomod/ESL" || goto :error
cmake --install build --component "BSA" --prefix "package/Fomod/BSA" || goto :error
cmake --install build --component "Loose" --prefix "package/Fomod/Loose" || goto :error
cmake --install build --component "Data" --prefix "package/Fomod/Data" || goto :error
cmake --install build --component "SDK" --prefix "package/SDK" || goto :error

pushd package
pushd Fomod
7z a -r -t7Z "..\MCM.Helper.7z"
popd
pushd SDK
7z a -r -t7Z "..\MCM.SDK.7z"
popd
popd

goto :EOF

:error
exit /b %errorlevel%
