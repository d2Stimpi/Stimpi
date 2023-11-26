-- Stimpi-ScriptCore - C# scripting
project "Stimpi-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"
	
	targetdir ("../resources/scripts")
	objdir ("../resources/scripts/intermediates")
	

	files
	{ 
		"../%{prj.name}/Source/**.cs",
		"../%{prj.name}/Properties/**.cs"
	}
	
	filter "configurations:Debug"
		optimize "Off"
		symbols "Default"
	
	filter "configurations:Release"
		optimize "On"
		symbols "Default"
		
	filter "configurations:Dist"
		optimize "Full"
		symbols "Off"