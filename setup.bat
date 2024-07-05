@echo off

set CMAKE_EXECUTABLE=cmake

if not exist build mkdir build

cd build

%CMAKE_EXECUTABLE% -G "Visual Studio 17 2022" ..

REM Vérifiez si cmake a réussi
if %errorlevel% neq 0 (
    echo Project generation failed!
    pause
    exit /b %errorlevel%
)

cd ..

xcopy /Y build\*.sln .

move build\DevilEngine.vcxproj Engine\
move build\DevilEngine.vcxproj.filters Engine\

setlocal enabledelayedexpansion
set "slnfile=DevilEngine.sln"
set "searchpath=DevilEngine.vcxproj"
set "replacepath=Engine\DevilEngine.vcxproj"

if exist %slnfile%.tmp del %slnfile%.tmp

for /f "delims=" %%i in (%slnfile%) do (
    set "line=%%i"
    set "modifiedline=!line:%searchpath%=%replacepath%!"
    echo !modifiedline!>> %slnfile%.tmp
)

move /Y %slnfile%.tmp %slnfile%
endlocal

rmdir /S /Q build

dotnet sln DevilEngine.sln remove ALL_BUILD.vcxproj
dotnet sln DevilEngine.sln remove ZERO_CHECK.vcxproj

powershell -Command "(Get-Content -path ./Engine/DevilEngine.vcxproj) -replace 'DevilEngine.dir', 'bin' | Set-Content -path ./Engine/DevilEngine.vcxproj"

echo Project DevilEngine generated! You can now close this window!

pause