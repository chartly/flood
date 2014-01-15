-- This is the starting point of the build scripts for the project.
-- It defines the common build settings that all the projects share
-- and calls the build scripts of all the sub-projects.

dofile "Helpers.lua"

ImportConfigs()

print("Generating the build configuration 'Build.h'")
local conf = GenerateBuildConfig(config)
WriteConfigToFile(conf, "../Build.h")

solution "Flood"
	
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	--defaultplatform "x32"
	
	flags { common_flags }
	language "C++"
	framework "4.5"
	
	location (builddir)
	objdir (builddir .. "/obj/")
	targetdir (libdir)
	libdirs { libdir }
	
	debugdir (bindir)
	startproject "Runtime"

	-- Build configuration options
	
	configuration "Release"
		flags { "Optimize" }

	configuration "Debug"
		defines { "DEBUG" }
		
	configuration {}

	group "Native"
	
		dofile( srcdir .. "/Core/Core.lua")
		dofile( srcdir .. "/Resources/Resources.lua")
		dofile( srcdir .. "/Graphics/Graphics.lua")
		dofile( srcdir .. "/Engine/Engine.lua")

	group "DIT"

		dofile( srcdir .. "/CS350/CS350.lua")
        
	group "Tests"

		dofile( srcdir .. "/Tests/UnitTests.lua")
