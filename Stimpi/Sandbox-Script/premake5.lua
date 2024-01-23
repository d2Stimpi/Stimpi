-- Sandbox-ScriptCore - C# scripting
project "Sandbox-Script"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"
	
	targetdir ("../resources/scripts")
	objdir ("../resources/scripts/intermediates")
	
	dependson { "Stimpi-ScriptCore" }

	files
	{ 
		"../%{prj.name}/Source/**.cs",
		"../%{prj.name}/Properties/**.cs"
	}
	
	links { "Stimpi-ScriptCore" }

	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"
	
	filter "configurations:Release"
		optimize "On"
		symbols "Default"
		
	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"