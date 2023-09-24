#pragma once

#include "Stimpi/Core/Core.h"

#include "Stimpi/Scene/Component.h"

#include <yaml-cpp/yaml.h>

template<>
struct YAML::convert<Stimpi::TagComponent>
{
	static YAML::Node encode(const Stimpi::TagComponent& tag)
	{
		YAML::Node node;
		node["Tag"] = tag.m_Tag;
		return node;
	}

	static bool decode(const YAML::Node& node, Stimpi::TagComponent& tag)
	{
		tag.m_Tag = node["Tag"].as<std::string>();
		return true;
	}
};

template<>
struct YAML::convert<Stimpi::TransformComponent>
{
	static YAML::Node encode(const Stimpi::TransformComponent& transform)
	{
		YAML::Node node;
		node["Transform"] = transform.m_Transform;
		return node;
	}

	static bool decode(const YAML::Node& node, Stimpi::TransformComponent& transform)
	{
		transform.m_Transform = node["Transform"].as<glm::vec4>();
		return true;
	}
};

template<>
struct YAML::convert<glm::vec4>
{
	static YAML::Node encode(const glm::vec4& vec)
	{
		YAML::Node node;
		node["glm::vect4"].push_back(vec.x);
		node["glm::vect4"].push_back(vec.y);
		node["glm::vect4"].push_back(vec.z);
		node["glm::vect4"].push_back(vec.w);
		return node;
	}

	static bool decode(const YAML::Node& node, glm::vec4& vec)
	{
		vec.x = node["glm::vect4"][0].as<float>();
		vec.y = node["glm::vect4"][1].as<float>();
		vec.z = node["glm::vect4"][2].as<float>();
		vec.w = node["glm::vect4"][3].as<float>();
		return true;
	}
};