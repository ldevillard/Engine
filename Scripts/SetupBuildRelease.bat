@echo off

set SRC_PATH=..\Engine\

set DEST_PATH=..\x64\Release\

echo delete the shaders and resources folders from the destination folder
rmdir /S /Q "%DEST_PATH%\shaders"
rmdir /S /Q "%DEST_PATH%\resources"

echo copy the shaders and resources folders from the source folder to the destination folder
xcopy /E /I "%SRC_PATH%\shaders" "%DEST_PATH%\shaders"
xcopy /E /I "%SRC_PATH%\resources" "%DEST_PATH%\resources"

echo done
pause