#pragma once

#include "Stimpi/Core/Core.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	struct CollisionPoint
	{
		glm::vec2 m_Position;

		CollisionPoint() = default;
		CollisionPoint(const CollisionPoint& point) = default;
	};

	struct Collision
	{
		uint32_t m_Owner = 0;				// Collision data owner Entity ID
		uint32_t m_ColliderEntityID = 0;	// Event ID that owner collided with

		CollisionPoint m_Point;

		Collision() = default;
		Collision(const Collision& collision) = default;
		Collision(uint32_t colliderID)
			: m_ColliderEntityID(colliderID)
		{
		}
	};

	class ST_API Physics
	{
	public:
		static void ShowColliderOutline(bool enable);
		static bool ShowColliderOutlineEnabled();
	};
}