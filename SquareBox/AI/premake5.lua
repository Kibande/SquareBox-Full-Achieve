project "AI"
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
		"_CRT_SECURE_NO_WARNINGS",
		"GLEW_STATIC"
	}

	includedirs
	{
	 "$(SolutionDir)/",
	  "$(SolutionDir)/vendor/include/",
	  "$(SolutionDir)/vendor/SDL2-2.30.2/include/",
	  "$(SolutionDir)/boost_1_73_0/",
	  "$(SolutionDir)/Glew/glew-2.2.0/include/",
	  "$(SolutionDir)/Glfw/glfw-3.3.2/include/"
	}


	links
	{
		
	}

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		defines 
		{
			"SQBAI_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines { "SQBAI_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "SQBAI_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "SQBAI_DIST" }
		--runtime "Dist"
		optimize "On"