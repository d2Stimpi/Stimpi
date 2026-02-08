-- Sandbox-ScriptCore - C# scripting
project "Demo-Script"
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

	-- Current path will be the %targetdir before executing postbuildcommands
	postbuildcommands
	{
		("{COPY} ../%{cfg.buildtarget.abspath} ../../assets/scripts")
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