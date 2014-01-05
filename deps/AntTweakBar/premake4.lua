project "anttweakbar"

	SetupNativeDependencyProject()
		
	local version = "1.16"
	local repo = "http://anttweakbar.sourceforge.net/doc/"
		
  kind "SharedLib"
  
  includedirs { "include/AntTweakBar" }

  pchheader "TwPrecomp.h"
  pchsource "src/TwPrecomp.cpp"

  files
  {
    "premake4.lua",
    "include/AntTweakBar/*.h",
    "src/*.cpp"
  }

  excludes 
  {
    "include/AntTweakBar/d3d10vs2003.h",
    "include/AntTweakBar/MiniGLUT*",
    "include/AntTweakBar/MiniSDL*",
    "include/AntTweakBar/TwDirect3D*",
    "src/TwDirect3D*"
  }

  defines
  {
    "TW_EXPORTS",
    "_USRDLL"
  }
  
  configuration "windows"
    files
    {
      "src/res/*",
      "src/*.rc"
    }

    defines { "_WINDOWS", "WIN32" }