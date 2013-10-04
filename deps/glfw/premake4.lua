project "glfw3"

	SetupNativeDependencyProject()
		
	local version = "3.0.4"
	local repo = "https://github.com/glfw/glfw"
		
  kind "StaticLib"
  
  includedirs { "include" }
  files
  {
    "premake4.lua",
    "include/*.h",
    "src/internal.h",
    "src/clipboard.c",
    "src/context.c",
    "src/gamma.c",
    "src/init.c",
    "src/input.c",
    "src/joystick.c",
    "src/monitor.c",
    "src/time.c",
    "src/window.c"
  }

  defines
  {
    "_GLFW_USE_OPENGL",
    "GLFW_EXPOSE_NATIVE_WIN32"
  }
  
  configuration "windows"
    files 
    {
      "src/win32_platform.h",
      "src/wgl_platform.h",
      "src/wgl_context.c",
      "src/win32_*.c"
    }

    defines
    {
      "_GLFW_WIN32",
      "_GLFW_WGL"
    }