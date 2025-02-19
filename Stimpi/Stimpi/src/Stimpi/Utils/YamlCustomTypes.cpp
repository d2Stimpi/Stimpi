#include "stpch.h"
#include "Stimpi/Utils/YamlCustomTypes.h"

// glm types

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& vec)
{
	out << YAML::BeginSeq;
	out << vec.x << vec.y;
	out << YAML::EndSeq;
	return out;
}