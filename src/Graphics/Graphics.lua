local boost = "C:/Users/Corbin/Desktop/Dev/thirdparty/boost/boost_1_55_0/"

project "Graphics"

	kind "ConsoleApp"

	SetupNativeProjects()

	files
	{
		"**.lua",
		"**.h",
		"**.cpp",
		"../../bin/*.glsl"
	}

	excludes { "GLFW/*InputManager.*" }

	defines { Engine.defines, "GLEW_MX" }

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
	
	libdirs { Engine.libdirs }

	links { Engine.name, Engine.links }

	deps { "glew", "glfw3", "anttweakbar" }

	configuration "windows"
		links { "gdi32", "opengl32", "glu32", "anttweakbar" }