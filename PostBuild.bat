@echo off

for /d %%I in ("dist\SkyrimSE\*") do rmdir /s /q "%%I"
for %%I in ("dist\SkyrimSE\*") do (
  if NOT "%%~nxI" == "MCMHelper.esp" del %%I
)

for /d %%I in ("dist\SkyrimVR\*") do rmdir /s /q "%%I"
for %%I in ("dist\SkyrimVR\*") do (
  if NOT "%%~nxI" == "MCMHelper.esp" del %%I
)
