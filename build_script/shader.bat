@echo off
setlocal enabledelayedexpansion

set "SOURCE_DIR=C:\D\code\c_game\Shaders"
set "DEST_DIR=C:\D\code\c_game\run\Content\Shaders"

for /r "%SOURCE_DIR%" %%d in (.) do (
    set "SUB_DIR=%%~dpd"
    set "REL_DIR=!SUB_DIR:%SOURCE_DIR%=!"
    mkdir "%DEST_DIR%!REL_DIR!" >nul 2>&1
)

for /r "%SOURCE_DIR%" %%f in (*.*) do (
    set "SRC_FILE=%%f"
    set "REL_PATH=%%~pf"
    set "DEST_FILE=!SRC_FILE:%SOURCE_DIR%=%DEST_DIR%!.spv"
    glslc "!SRC_FILE!"  -o "!DEST_FILE!" 
)

echo Compilation complete.
exit
