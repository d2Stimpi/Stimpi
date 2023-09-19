#pragma once

#include "Stimpi/Core/Core.h"

#include <glm/glm.hpp>

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
}