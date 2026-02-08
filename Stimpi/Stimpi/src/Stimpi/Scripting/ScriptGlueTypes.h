#pragma once

namespace Stimpi
{
	constexpr const char* s_EntityType = "Stimpi.Entity";
	constexpr const char* s_ComponentType = "Stimpi.Component";
	constexpr const char* s_ComponentTypeBaseStr = "struct Stimpi::";
	// Scripting visible/available Component types
	constexpr const char* s_QuadComponentType = "Stimpi.QuadComponent";

	constexpr const char* s_CoreNamespace	= "Stimpi";
	constexpr const char* s_AttributeLookup = "AttributeLookup";

	// AttributeLookup class method names
	constexpr const char* s_HasSerializeFieldAttribute		= "HasSerializeFieldAttribute";
	constexpr const char* s_HasScriptOrderAttribute			= "HasScriptOrderAttribute";
	constexpr const char* s_GetScriptOrderAttributeValue	= "GetScriptOrderAttributeValue";
}