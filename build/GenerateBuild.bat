@echo off
goto menu

:menu
echo DigiPen Assignment Project Generator:
echo.
echo [0] Clean
echo [1] Visual C++ 2012
echo [2] Visual C++ 2012 (deps only)
echo.

:choice
set /P C="Choice: "
if "%C%"=="2" goto vs2012deps
if "%C%"=="1" goto vs2012
if "%C%"=="0" goto clean

:clean
"../tools/premake4" --file=premake/premake4-dit.lua clean
"../tools/premake4" --file=premake/premake4-deps.lua clean
goto quit

:vs2012
"../tools/premake4" --file=premake/premake4-dit.lua vs2012
"../tools/premake4" --file=premake/premake4-deps.lua vs2012
goto quit

:vs2012deps
"../tools/premake4" --file=premake/premake4-deps.lua vs2012
goto quit

:quit
pause
:end