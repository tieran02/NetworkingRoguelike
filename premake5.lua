require "codeblocks"

workspace "networking_project"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "client"
	location "client"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++11"

	targetdir "bin/%{cfg.buildcfg}"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files { "client/src/**.*" }

	filter "system:windows"
		systemversion "latest"
		includedirs { "dependencies/windows/SFML/include",
					  "client/src"}
		libdirs { "dependencies/windows/SFML/lib" }
		links
		{	
			"sfml-graphics",
			"sfml-window",
			"sfml-system",
			"sfml-audio",
			"sfml-network"
		}

		postbuildcommands
		{
			("{COPY} %{wks.location}dependencies/windows/SFML/bin ../bin/" .. outputdir .. "/client"),
			("{COPY} %{prj.location}resources ../bin/" .. outputdir .. "/client/resources")
		}

	filter "system:linux"	
		cppdialect "C++11"
		systemversion "latest"
		includedirs { "dependencies/linux/SFML/include",
					  "client/src"}
		libdirs { "dependencies/linux/SFML/lib" }
		linkoptions { '-Wl,-rpath=\\$$ORIGIN' }
		links
		{	
			"jpeg",
			"sfml-graphics",
			"sfml-window",
			"sfml-system",
			"sfml-audio",
			"sfml-network"
		}

		postbuildcommands
		{
			("{COPY} %{wks.location}dependencies/linux/SFML/lib/bin ../bin/" .. outputdir .. "/client"),
			("{COPY} %{prj.location}resources ../bin/" .. outputdir .. "/client/resources")
		}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"


	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
