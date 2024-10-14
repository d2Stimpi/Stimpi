-- Stimpi - Core engine
project "Stimpi"
	kind "SharedLib"
	architecture "x64"
	language "C++"
	
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	
	pchheader "stpch.h"
	pchsource "src/stpch.cpp"

	files
	{ 
		"../%{prj.name}/src/**.h",
		"../%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"../%{prj.name}/src",
		"../%{prj.name}/src/Stimpi/Vendor",
		"../%{prj.name}/vendor/Box2D/include",
		"../vendor/SDL2-2.0.20/include",
		"../vendor/spdlog/include",
		"../vendor/glm-0.9.9.7/glm",
		"../vendor/yaml-cpp/include",
		"../vendor/stbimg/",
		"../GLAD/include",

		"%{IncludeDir.mono}"
	}

	libdirs { "../vendor/SDL2-2.0.20/lib/x64" }		links { "SDL2" }
	libdirs { "../vendor/yaml-cpp/Debug" }			links { "yaml-cppd" }
	libdirs { "../vendor/glfw/lib-vc2019" }			links { "glfw3" }
	libdirs { "../bin/" .. outputdir .. "/GLAD" }	links { "GLAD" }
	libdirs { "../bin/" .. outputdir .. "/Box2D" }	links { "Box2D" }

	libdirs { "%{LibraryDir.mono}" }	links { "%{Library.mono}" }
	links { "%{Library.Winmm}" }
	links { "%{Library.Ws2}" }
	links { "%{Library.Bcrypt}"}
	links { "%{Library.Version}"}
	links { "%{Library.Comctl32}"}

	defines
	{ 
		"STIMPI_BUILD_DLL" 
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"
	
		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Application"),
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Runtime"),
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Editor"),
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Testing")
		}

	filter "configurations:Debug"
		defines "DEBUG"
		symbols "On"
	
	filter "configurations:Release"
		defines "NDEBUG"
		optimize "On"
		
	filter "configurations:Dist"
		defines "NDEBUG"
		optimize "On"

	filter { "system:windows", "configurations:Release" }
		buildoptions "/MD"
		
	filter { "system:windows", "configurations:Debug" }
		buildoptions "/MDd"