-- Stimpi-ScriptCore - C# scripting
project "Stimpi-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"
	
	targetdir ("../resources/scripts")
	objdir ("../resources/scripts/intermediates")
	
	nuget
	{
		"Microsoft.CodeAnalysis.Analyzers:3.3.4",
		"Microsoft.CodeAnalysis.Common:4.9.2",
		"Microsoft.CodeAnalysis.CSharp:4.9.2",
		"System.Buffers:4.5.1",
		"System.CodeDom:8.0.0",
		"System.Collections.Immutable:8.0.0",
		"System.Memory:4.5.5",
		"System.Numerics.Vectors:4.5.0",
		"System.Reflection.Metadata:8.0.0",
		"System.Runtime.CompilerServices.Unsafe:6.0.0",
		"System.Text.Encoding.CodePages:8.0.0",
		"System.Threading.Tasks.Extensions:4.5.4"
	}

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