local boost = "C:/Users/Corbin/Desktop/Dev/thirdparty/boost/boost_1_55_0/"

Graphics = {}
Graphics.name = "Graphics"
Graphics.shared = true
Graphics.defines = {}

project "Graphics"

	kind "StaticLib"

	SetupNativeProjects()

	files
	{
		path.join(incdir,"Graphics/**.h"),
		"**.lua",
		"**.h",
		"**.cpp",
		"../../bin/*.glsl"
	}

	excludes { "GLFW/*InputManager.*" }

	table.insert(Graphics.defines, { Core.defines })

	defines { "GLEW_MX" }

	includedirs
	{
		incdir,
		srcdir,
		path.join(depsdir, "glew/glew/include/"),
		path.join(depsdir, "glfw/include/"),
		path.join(depsdir, "glm/"),
		path.join(depsdir, "miniformat/"),
		path.join(depsdir, "anttweakbar/include/"),
		boost
	}
	
	libdirs { Core.libdirs }

	links { Core.name, Core.links }

	deps { "glew", "glfw3", "anttweakbar" }

	configuration "windows"
		links { "gdi32", "opengl32", "glu32", "anttweakbar" }