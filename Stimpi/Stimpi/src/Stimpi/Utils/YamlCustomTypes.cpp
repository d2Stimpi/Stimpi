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

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
{
	out << YAML::BeginSeq;
	out << vec.x << vec.y << vec.z;
	out << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& vec)
{
	out << YAML::BeginSeq;
	out << vec.x << vec.y << vec.z << vec.w;
	out << YAML::EndSeq;
	return out;
}