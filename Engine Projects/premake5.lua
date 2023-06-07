workspace "Engine Projects"
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

--Include directories in this directory
IncludeDir={"."}

-- Projects 
group "Dependencies"
	include ""
group ""

-- Creating the  projects 
include "Empty-Project"
include "FairWatt"
include "Galaga-Game"
include "Mean-While-In-Russia-Game"
include "Platformer"
include "Snake-Game"
include "SquareBox-SandBox"
include "Super-Mario-Game"
include "Wenova-Game"
include "Zombie-Game"
include "SquareBox-Editor"
include "SquareBox-Examples"
include "Lua-Scripted"


workspace "Engine Projects"
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

--Include directories in this directory
IncludeDir={"."}

-- Projects 
group "Dependencies"
	include ""
group ""

-- Creating the  projects 
include "Empty-Project"
include "FairWatt"
include "Galaga-Game"
include "Mean-While-In-Russia-Game"
include "Platformer"
include "Snake-Game"
include "SquareBox-SandBox"
include "Super-Mario-Game"
include "Wenova-Game"
include "Zombie-Game"
include "SquareBox-Editor"
include "SquareBox-Examples"
include "Lua-Scripted"





