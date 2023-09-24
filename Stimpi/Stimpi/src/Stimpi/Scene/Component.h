#pragma once

#include "Stimpi/Core/Core.h"

#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

namespace Stimpi
{
	struct TagComponent
	{
		std::string m_Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: m_Tag(tag) {}
	};

	struct TransformComponent
	{
		glm::vec4 m_Transform;

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec4& transform)
			: m_Transform(transform) {}
	};

	struct QuadComponent
	{
		float m_X;
		float m_Y;
		float m_Width;
		float m_Height;

		QuadComponent() = default;
		QuadComponent(const QuadComponent&) = default;
		QuadComponent(const glm::vec4& quad)
			: m_X(quad.x), m_Y(quad.y), m_Width(quad.z), m_Height(quad.w) {}

		operator glm::vec4() const { return glm::vec4(m_X, m_Y, m_Width, m_Height); }
	};
}

	