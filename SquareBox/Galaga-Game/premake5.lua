project "Galaga-Game"
	kind "ConsoleApp"
	language "C++"

	staticruntime "on"

	local outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


	targetdir ("../bin/".. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/".. outputdir .."/%{prj.name}")



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
	  "$(SolutionDir)/Glew/glew-2.2.0/include/",
	  "$(SolutionDir)/Glfw/glfw-3.3.2/include/",
	  "$(SolutionDir)/vendor/include/ffmpeg"

	}

	libdirs 
	{   
		"$(SolutionDir)/bin/libs/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"
	}

	links
	{
		"SquareBox-Core",
		"SquareBox-Core.lib"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		defines 
		{
			"SQBSANDBOX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines { "SQBSANDBOX_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "SQBSANDBOX_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "SQBSANDBOX_DIST" }
		--runtime "Dist"
		optimize "On"