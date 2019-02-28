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
					  "client/src",
					  "shared_network/include"}
		libdirs { "dependencies/windows/SFML/lib",
				  "bin/" .. outputdir .. "shared_network"}
		links
		{	
			"sfml-graphics",
			"sfml-window",
			"sfml-system",
			"sfml-audio",
			"sfml-network",
			"shared_network"
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
					  "client/src",
					  "shared_network/include"}
					  
		libdirs { "dependencies/linux/SFML/lib",
				  "bin/" .. outputdir .. "shared_network"}
		linkoptions { '-Wl,-rpath=\\$$ORIGIN' }
		links
		{	
			"jpeg",
			"sfml-graphics",
			"sfml-window",
			"sfml-system",
			"sfml-audio",
			"sfml-network",
			"shared_network"
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

project "server"
	location "server"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++11"

	targetdir "bin/%{cfg.buildcfg}"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files { "server/src/**.*" }

	filter "system:windows"
		systemversion "latest"
		includedirs { "dependencies/windows/SFML/include",
					  "server/src",
					  "shared_network/include"}
		libdirs { "dependencies/windows/SFML/lib",
				  "shared_network/src" }
		links
		{	
			"sfml-system",
			"sfml-network",
			"shared_network"
		}

		postbuildcommands
		{
			("{COPY} %{wks.location}dependencies/windows/SFML/bin ../bin/" .. outputdir .. "/server")
		}

	filter "system:linux"	
		cppdialect "C++11"
		systemversion "latest"
		includedirs { "dependencies/linux/SFML/include",
					  "server/src",
					  "shared_network/include"}
		libdirs { "dependencies/linux/SFML/lib" }
		linkoptions { '-Wl,-rpath=\\$$ORIGIN' }
		links
		{	
			"sfml-system",
			"sfml-network",
			"shared_network"
		}

		postbuildcommands
		{
			("{COPY} %{wks.location}dependencies/linux/SFML/lib/bin ../bin/" .. outputdir .. "/server")
		}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"


	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		
project "shared_network"
location "shared_network"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	systemversion "latest"

	targetdir "bin/%{cfg.buildcfg}"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files { "shared_network/src/**.*",
			"shared_network/include/shared/**.*"	}

	filter "system:windows"
		includedirs { "dependencies/windows/SFML/include",
					  "%{prj.name}/include/shared"}
		libdirs { "dependencies/windows/SFML/lib" }
		links
		{	
			"sfml-system",
			"sfml-network"
		}

	filter "system:linux"	
		includedirs { "dependencies/linux/SFML/include",
					  "%{prj.name}/include/shared"}
		libdirs { "dependencies/linux/SFML/lib" }
		links
		{	
			"sfml-system",
			"sfml-network"
		}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"


	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
