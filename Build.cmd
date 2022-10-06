@echo off

cmake --preset vs2022-windows --check-stamp-file "build/CMakeFiles/generate.stamp" || goto :error
cmake --build build --config Release || goto :error
cmake --install build --prefix "package/SkyrimSE" || goto :error
cmake --install build --component "SDK" --prefix "package/SDK" || goto :error

cmake --preset vs2022-windows-vr --check-stamp-file "buildVR/CMakeFiles/generate.stamp" || goto :error
cmake --build buildVR --config Release || goto :error
cmake --install buildVR --prefix "package/SkyrimVR" || goto :error

pushd package
pushd SkyrimSE
7z a -r -t7Z "..\MCM.Helper.SE.7z"
popd
pushd SkyrimVR
7z a -r -t7Z "..\MCM.Helper.VR.7z"
popd
pushd SDK
7z a -r -t7Z "..\MCM.SDK.7z"
popd
popd

goto :EOF

:error
exit /b %errorlevel%
