@echo off
goto menu

:menu
echo DigiPen Assignment Project Generator:
echo.
echo [0] Clean
echo [1] Visual C++ 2013
echo [2] Visual C++ 2012
echo.

:choice
set /P C="Choice: "
if "%C%"=="2" goto vs2012
if "%C%"=="1" goto vs2013
if "%C%"=="0" goto clean

:clean
"../tools/premake5" --file=premake/premake4-dit.lua clean
"../tools/premake5" --file=premake/premake4-deps.lua clean
goto quit

:vs2013
"../tools/premake5" --file=premake/premake4-dit.lua vs2013
"../tools/premake5" --file=premake/premake4-deps.lua vs2013
goto quit

:vs2012
"../tools/premake5" --file=premake/premake4-dit.lua vs2012
"../tools/premake5" --file=premake/premake4-deps.lua vs2012
goto quit

:quit
pause
:end