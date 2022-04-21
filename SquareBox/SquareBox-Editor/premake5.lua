project "SquareBox-Editor"
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
	  
	  "$(SolutionDir)/vendor/lib/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/",
	  "$(SolutionDir)/bin/libs/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"
	}

	links
	{
		"SquareBox-Core",
		"SquareBox-Core.lib",
		"ImGui",
		"Efsw",
		"ImGui.lib",
		"Efsw.lib"
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		defines 
		{
			"SQBEDITOR_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines { "SQBEDITOR_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "SQBEDITOR_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "SQBEDITOR_DIST" }
		--runtime "Dist"
		optimize "On"