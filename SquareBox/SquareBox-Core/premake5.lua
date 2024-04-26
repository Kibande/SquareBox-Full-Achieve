project "SquareBox-Core"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	local outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	targetdir ("../bin/libs/".. outputdir)
	objdir ("../bin-int/libs/".. outputdir)

	--pchheader "sqbpch.h"
	--pchsource "sqbpch.cpp"

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
	  "$(SolutionDir)/Glew/glew-2.2.0/include/",
	  "$(SolutionDir)/Glfw/glfw-3.3.2/include/",
	  "$(SolutionDir)/vendor/include/ffmpeg"

	}

	libdirs
	{
	    "$(SolutionDir)/bin/libs/"..outputdir.."/",
		"$(SolutionDir)/vendor/lib/%{cfg.architecture}/",
		"$(SolutionDir)/vendor/SDL2_mixer-2.8.0/lib/%{cfg.architecture}/",
		"$(SolutionDir)/vendor/SDL2_ttf-2.22.0/lib/%{cfg.architecture}/",
		"$(SolutionDir)/vendor/SDL2-2.30.2/lib/%{cfg.architecture}/"
	}



	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		defines 
		{
			"SQBCORE_PLATFORM_WINDOWS",
			"_SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING"
		}

		links
		{
			"AI",
			"Animation-System",
			"Audio-System",
			"Camera",
			"GWOM",
			"ImGui",
			"MathLib",
			"Input-Manager",
			"Network-Manager",
			"Shapes",
			"GUI",
--			"Media-Player",
			"SquareBox-Logger",
			"Physics-Collision-Engine",
			"Render-Engine",
			"Asset-Manager",
			"Box2D",
			"Lua",
			"Efsw",
			"AI.lib",
			"Animation-System.lib",
			"Audio-System.lib",
			"Camera.lib",
			"GWOM.lib",
			"ImGui.lib",
			"MathLib.lib",
			"Input-Manager.lib",
			"Network-Manager.lib",
			"Shapes.lib",
			"GUI.lib",
--			"Media-Player.lib",
			"SquareBox-Logger.lib",
			"Physics-Collision-Engine.lib",
			"Render-Engine.lib",
			"Asset-Manager.lib",
			"Box2D.lib",
			"Lua.lib",
			"Efsw.lib",
			"SDL2main.lib",
			"SDL2.lib",
			"SDL2_mixer.lib",
			"SDL2_ttf.lib",
			"opengl32.lib",
			"glfw3.lib"
		}
	-- configurations for static glew
	filter "configurations:Debug"
		links{"glew32sd.lib"}

	filter "configurations:Release"
		links{"glew32s.lib"}

--	filter "platforms:x86"
--		links{"libvlc.lib","libvlccore.lib"}


--	filter "platforms:x86_64"
--		links{"libvlc.x64.lib","libvlccore.x64.lib"}

	filter "configurations:Debug"
		defines { "SQBCORE_DEBUG" }
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines { "SQBCORE_RELEASE" }
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines { "SQBCORE_DIST" }
		--runtime "Dist"
		optimize "On"