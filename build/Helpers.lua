-- This module checks for the all the project dependencies.

action = _ACTION or ""

libdir = path.getabsolute(path.join(action, "lib"));
gendir = path.getabsolute(path.join(action, "gen"));

local depsdir = "../deps/"
local builddir = path.getabsolute(action);

common_flags = { "Unicode", "Symbols", "NoExceptions", "NoRTTI" }
msvc_buildflags = { "/wd4190", "/wd4996", "/wd4530" }
gcc_buildflags = { "-Wno-invalid-offsetof", "-std=gnu++11" }

function debug(msg)
	-- print(msg)
end

function SetupLibPaths(lib) 
	c = configuration {}
	
	local src = depsdir .. lib .. "/src/"
	local include = depsdir .. lib .. "/include/"
	
	if os.isdir(path.getabsolute(include)) then
		debug("Including lib", lib, "from", include)
		includedirs { include }
	elseif os.isdir(path.getabsolute(src)) then
		debug("Including lib", lib, "from", src)
		includedirs { src }
	else
		debug("Including lib", lib, "from", depsdir .. lib)
		includedirs { depsdir .. lib }
	end

	local libpath = depsdir .. lib .. "/lib/" .. action
	if os.isdir(path.getabsolute(libpath)) then
		libdirs { libpath }
	end
	
	libdirs { depsdir .. "build/" .. action .. "/lib" }
	
	configuration(c)
end



function SetupNativeProjects()
	location (path.join(builddir, "projects"))

	c = configuration "Debug"
		defines { "DEBUG" }
		targetsuffix "_d"
		
	configuration "Release"
		defines { "NDEBUG" }
	
	configuration(c)
end

function SetupDependencyProject()
	location (path.join(builddir, "deps"))
end



function SetupLibLinks(lib)
	c = configuration "Debug"
		links { lib .. "_d" }
	configuration "Release"
		links { lib }
	configuration(c)
end

function builddeps(deps)
end

function deps(dep)
	for i,lib in ipairs(dep) do
		if type(lib) == "table" then
			deps(lib)
		else
			SetupLibPaths(lib)
			SetupLibLinks(lib)
		end
	end
end

function GenerateBuildConfig(conf)
	local contents = ""
	
	for key,value in pairs(conf) do
		local define = string.format("#define ENABLE_%s", key)
		if value then
			--print(string.format("Enabling %s", key))
		else
			define = "//" .. define
		end
		contents = contents .. define .. "\n"
	end
	
	return contents
end

function WriteConfigToFile(conf, path)
	file = io.open(path, "w")
	file:write(conf)
	file:close()
end