workspace "SquareBox"
	platforms { "x86", "x86_64" }
	startproject "SquareBox-SandBox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}
--local outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to SandBox-Core
IncludeDir={"."}

-- Projects 
group "Dependencies"
	include ""
group ""

-- Creating the  projects 
include "AI"
include "Animation-System"
include "Audio-System"
include "Camera"
include "Efsw"
include "GWOM"
include "Box2D"
include "Lua"
include "ImGui"
include "MathLib"
include "Network-Manager"
include "Input-Manager"
include "Asset-Manager"
include "Shapes"
include "Physics-Collision-Engine"
include "Render-Engine"
include "Media-Player"
include "SquareBox-Core"
include "SquareBox-Editor"
include "SquareBox-Logger"
include "SquareBox-SandBox"
include "SquareBox-Examples"
include "Zombie-Game"
include "Ball-Game"



workspace "SquareBox"
	filter "platforms:x86"
		architecture "x86"
	filter "platforms:x86_64"
		architecture "x86_64"
	startproject "SquareBox-SandBox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}
--local outputdir ="%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--Include directories relative to SandBox-Core
IncludeDir={"."}

-- Projects 
group "Dependencies"
	include ""
group ""

-- Creating the  projects 
include "AI"
include "Animation-System"
include "Audio-System"
include "Camera"
include "Efsw"
include "GWOM"
include "Box2D"
include "Lua"
include "ImGui"
include "MathLib"
include "Input-Manager"
include "Network-Manager"
include "Asset-Manager"
include "Shapes"
include "Physics-Collision-Engine"
include "Render-Engine"
include "Media-Player"
include "SquareBox-Core"
include "SquareBox-Editor"
include "SquareBox-Logger"
include "SquareBox-SandBox"
include "SquareBox-Examples"
include "Zombie-Game"
include "Ball-Game"





