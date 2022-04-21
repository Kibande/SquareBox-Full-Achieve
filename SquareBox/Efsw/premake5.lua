project "Efsw"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	local outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../bin/libs/".. outputdir)
	objdir ("../bin-int/libs/".. outputdir)




	files {"../vendor/include/efsw/*.cpp", "../vendor/include/efsw/platform/win/*.cpp","**.h","**.cpp" }

	vpaths {
		["Headers"] = { "**h" },
		["Sources"] = {"**.cpp","../vendor/include/efsw/**.cpp","../vendor/include/efsw/platform/win/*.cpp"},
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
		excludes { "../vendor/include/efsw/WatcherKqueue.cpp", "../vendor/include/efsw/WatcherFSEvents.cpp", "../vendor/include/efsw/WatcherInotify.cpp", "../vendor/include/efsw/FileWatcherKqueue.cpp", "../vendor/include/efsw/FileWatcherInotify.cpp", "../vendor/include/efsw/FileWatcherFSEvents.cpp" }
		defines 
		{
			"SQBEFSW_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines { "SQBEFSW_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "SQBEFSW_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "SQBEFSW_DIST" }
		--runtime "Dist"
		optimize "On"