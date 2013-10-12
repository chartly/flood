project "glew"

	SetupNativeDependencyProject()
		
	local version = "1.9.0"
	local repo = "https://github.com/martell/glew"
	
  kind "StaticLib"
  
  files 
  {
  	"glew/include/glew/glew.h",
  	"glew/src/*.c"
  }

  includedirs { "glew/include" }
  
  defines { "GLEW_BUILD", "GLEW_MX" }

  configuration "windows"
  	files { "glew/include/glew/wglew.h" }