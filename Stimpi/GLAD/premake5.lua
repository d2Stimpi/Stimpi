-- GLAD
project "GLAD"
	kind "StaticLib"
	architecture "x64"
	language "C++"
	
	targetdir ("../bin/" .. outputdir .. "/%{prj.name}")
	objdir ("../bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{ 
		"../%{prj.name}/src**.h",
		"../%{prj.name}/src/**.c"
	}

	includedirs
	{
		"../%{prj.name}/include"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"
	
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