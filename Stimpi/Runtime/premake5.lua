-- Application
project "Runtime"
	kind "ConsoleApp"
	architecture "x64"
	language "C++"
	
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	
	dependson { "Stimpi" }

	files
	{ 
		"../%{prj.name}/src/**.h",
		"../%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"../Stimpi/src",
		spdlog_inc,
		glm_inc,
		sdl_inc,
		entt_inc,
		yaml_inc
	}
	
	libdirs
	{
		"../vendor/SDL2-2.0.20/lib/x64",
		"../vendor/yaml-cpp/Debug"
	}

	links
	{
		"Stimpi",
		"SDL2",
		"SDL2main",
		"yaml-cppd"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		postbuildcommands
		{
			("{COPY} %{corelibpath} ../bin/" .. outputdir .. "/%{prj.name}"),
			("{COPY} %{Library.sdl2} ../bin/" .. outputdir .. "/%{prj.name}"),
			("{COPY} %{Library.yaml} ../bin/" .. outputdir .. "/%{prj.name}")
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