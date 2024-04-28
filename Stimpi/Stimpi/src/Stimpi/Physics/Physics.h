#pragma once

#include "Stimpi/Core/Core.h"

#include <glm/glm.hpp>

#define PHYS_MAXCONTACTPOINTS	2

namespace Stimpi
{
	struct Contact
	{
		glm::vec2 m_Points[PHYS_MAXCONTACTPOINTS] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		uint32_t m_PointCount = 0;

		Contact() = default;
		Contact(const Contact& contact) = default;
	};

	struct Collision
	{
		uint32_t m_Owner = 0;				// Collision data owner Entity ID
		uint32_t m_ColliderEntityID = 0;	// Event ID that owner collided with

		std::list<Contact> m_Contacts;
		uint32_t m_ConctactCount = 0;

		Collision() = default;
		Collision(const Collision& collision) = default;
		Collision(uint32_t colliderID)
			: m_ColliderEntityID(colliderID)
		{
		}

		static bool Compare(Collision* a, Collision* b);
	};

	class ST_API Physics
	{
	public:
		static void ShowColliderOutline(bool enable);
		static bool ShowColliderOutlineEnabled();

		static void ShowCollisionsContactPoints(bool enable);
		static bool ShowCollisionsContactPointsEnabled();

		static void AddActiveCollision(Collision* collision);
		static void RemoveActiveCollision(Collision* collision);
		static void UpdateActiveCollision(Collision* collision);
		static void ClearActiveCollisions();
		static std::vector<std::shared_ptr<Collision>>& GetActiveCollisions();
	};
}