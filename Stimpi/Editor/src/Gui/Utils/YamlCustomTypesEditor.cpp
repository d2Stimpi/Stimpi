#include "stpch.h"
#include "Gui/Utils/YamlCustomTypesEditor.h"

YAML::Emitter& operator<<(YAML::Emitter& out, const ImVec2& vec)
{
	out << YAML::BeginSeq;
	out << vec.x << vec.y;
	out << YAML::EndSeq;
	return out;
}