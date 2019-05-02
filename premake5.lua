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
	defines { "CLIENT" }


	targetdir "bin/%{cfg.buildcfg}"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files { "client/src/**.*" }
	includedirs {"client/src", "shared_network/include", "dependencies/spdlog/include", "dependencies/json/include" }
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
			("{COPY} %{prj.location}/resources ../bin/" .. outputdir .. "/client/resources"),
			("{COPY} %{wks.location}/shared_network/resources/entities.json ../bin/" .. outputdir .. "/client/resources"),
			("{COPY} %{wks.location}dependencies/windows/SFML/bin ../bin/" .. outputdir .. "/client")
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
			("cp -r %{prj.location}/resources ../bin/" .. outputdir .. "/client/resources"),
			("cp %{wks.location}/shared_network/resources/entities.json ../bin/" .. outputdir .. "/client/resources"),
			("cp -r %{wks.location}/dependencies/linux/SFML/lib/bin ../bin/*" .. outputdir .. "/client")
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
	defines { "SERVER" }


	targetdir "bin/%{cfg.buildcfg}"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	debugdir ("bin/" .. outputdir .. "/%{prj.name}")

	files { "server/src/**.*" }

	includedirs {"server/src", "shared_network/include", "dependencies/spdlog/include", "dependencies/json/include"}
	libdirs { "shared_network/src" }
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
		includedirs { "dependencies/windows/SFML/include" }
		libdirs { "dependencies/windows/SFML/lib" }

		postbuildcommands
		{
			("{COPY} %{wks.location}/shared_network/resources/ ../bin/" .. outputdir .. "/server/resources"),
			("{COPY} %{wks.location}dependencies/windows/SFML/bin ../bin/" .. outputdir .. "/server")
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
			("cp -r %{wks.location}/shared_network/resources/ ../bin/" .. outputdir .. "/server/resources"),
			("cp -r %{wks.location}/dependencies/linux/SFML/lib/bin/* ../bin/" .. outputdir .. "/server")
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

	includedirs { "%{prj.name}/include/shared", "dependencies/spdlog/include", "dependencies/json/include" }

	filter "system:windows"
		includedirs { "dependencies/windows/SFML/include" }
		libdirs { "dependencies/windows/SFML/lib" }
        links
		{
			"sfml-graphics",
			"sfml-system"
		}

	filter "system:linux"
		includedirs { "dependencies/linux/SFML/include" }
		libdirs { "dependencies/linux/SFML/lib" }
		links
		{
			"sfml-graphics",
			"sfml-system"
		}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"


	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
