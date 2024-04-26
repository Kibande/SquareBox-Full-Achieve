project "Asset-Manager"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	local outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../bin/libs/".. outputdir)
	objdir ("../bin-int/libs/".. outputdir)



	files
	{
		"**.h",
		"**.cpp",
	}

	vpaths {
		["Headers"] = { "**.h", "**.hpp" },
		["Sources*"] = {"**.c", "**.cpp"},
		["Docs"] = "**.md"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
	 "$(SolutionDir)/",
	  "$(SolutionDir)/vendor/include/",
	  "$(SolutionDir)/vendor/SDL2-2.30.2/include/",
	  "$(SolutionDir)/Glew/glew-2.2.0/include/"
	}

	libdirs
	{
	    "$(SolutionDir)/vendor/lib/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/",
	}

	links
	{
		
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		defines 
		{
			"SQBASSETMANAGER_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines { "SQBASSETMANAGER_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "SQBASSETMANAGER_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "SQBASSETMANAGER_DIST" }
		--runtime "Dist"
		optimize "On"