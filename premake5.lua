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
	systemversion "latest"

	targetdir "bin/%{cfg.buildcfg}"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files { "client/src/**.*" }
	includedirs {"client/src", "shared_network/include"}
	libdirs {"bin/" .. outputdir .. "shared_network"}
	links
	{	
		"sfml-graphics",
		"sfml-window",
		"sfml-system",
		"sfml-audio",
		"sfml-network",
		"shared_network"
	}


	filter "system:windows"
		systemversion "latest"
		includedirs { "dependencies/windows/SFML/include"}

		libdirs { "dependencies/windows/SFML/lib" }

		postbuildcommands
		{
			("{COPY} %{wks.location}dependencies/windows/SFML/bin ../bin/" .. outputdir .. "/client"),
			("{COPY} %{prj.location}resources ../bin/" .. outputdir .. "/client/resources")
		}

	filter "system:linux"	
		includedirs { "dependencies/linux/SFML/include" }
					  
		libdirs { "dependencies/linux/SFML/lib" }
		linkoptions { '-Wl,-rpath=\\$$ORIGIN' }
		links
		{	
			"jpeg"
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
	systemversion "latest"

	targetdir "bin/%{cfg.buildcfg}"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files { "server/src/**.*" }

	includedirs {"server/src", "shared_network/include"}
	libdirs { "shared_network/src" }
	links
	{	
		"sfml-system",
		"sfml-network",
		"shared_network"
	}

	filter "system:windows"
		includedirs { "dependencies/windows/SFML/include" }
		libdirs { "dependencies/windows/SFML/lib" }

		postbuildcommands
		{
			("{COPY} %{wks.location}dependencies/windows/SFML/bin ../bin/" .. outputdir .. "/server")
		}

	filter "system:linux"	
		includedirs { "dependencies/linux/SFML/include" }
		libdirs { "dependencies/linux/SFML/lib" }
		linkoptions { '-Wl,-rpath=\\$$ORIGIN' }

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

	includedirs { "%{prj.name}/include/shared" }
	links
	{	
		"sfml-system",
		"sfml-network"
	}

	filter "system:windows"
		includedirs { "dependencies/windows/SFML/include" }
		libdirs { "dependencies/windows/SFML/lib" }

	filter "system:linux"	
		includedirs { "dependencies/linux/SFML/include" }
		libdirs { "dependencies/linux/SFML/lib" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"


	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
