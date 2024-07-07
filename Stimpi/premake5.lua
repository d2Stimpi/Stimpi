-- Workspace - include all subproj
workspace "Stimpi"
	configurations { "Debug", "Release", "Dist" }
	architecture "x64"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	startproject "Editor"

	spdlog_inc = "../vendor/spdlog/include"
	glm_inc = "../vendor/glm-0.9.9.7/glm"
	sdl_inc = "../vendor/SDL2-2.0.20/include"
	entt_inc = "../Stimpi/src/Stimpi/Vendor"
	yaml_inc = "../vendor/yaml-cpp/include"
	imgui_inc = "../ImGUI"


include "Dependancies"

-- run other projects premake5.lua scripts
include "Application"
include "Runtime"
include "Editor"
include "Stimpi"

group "Dependencies"
	include "ImGUI"
	include "GLAD"
	include "Stimpi/vendor/Box2D"
group ""

group "Scripting"
	include "Stimpi-ScriptCore"
	include "Sandbox-Script"
group ""