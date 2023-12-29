#pragma once

namespace Stimpi
{
	struct Collision
	{
		uint32_t m_Owner = 0;				// Collision data owner Entity ID
		uint32_t m_ColliderEntityID = 0;	// Event ID that owner collided with

		Collision() = default;
		Collision(const Collision& collision) = default;
		Collision(uint32_t colliderID)
			: m_ColliderEntityID(colliderID)
		{
		}
	};
}