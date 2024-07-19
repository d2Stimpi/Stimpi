#pragma once

#include "Stimpi/Core/Core.h"

#include <glm/glm.hpp>

#define PHYS_MAXCONTACTPOINTS	2

namespace Stimpi
{
	/**
	 * Represents objects collision contact point
	 */
	struct Contact
	{
		glm::vec2 m_Point = { 0.0f, 0.0f };
		glm::vec2 m_ImpactVelocity = { 0.0f, 0.0f };

		Contact() = default;
		Contact(const Contact& contact) = default;
	};

	/**
	 * Collision data
	 */
	struct Collision
	{
		uint32_t m_Owner = 0;							// Collision data owner Entity ID
		uint32_t m_ColliderEntityID = 0;				// Event ID that owner collided with
		glm::vec2 m_ImpactVelocity = { 0.0f, 0.0f };
		std::list<Contact> m_Contacts;

		Collision() = default;
		Collision(const Collision& collision) = default;
		Collision(uint32_t colliderID)
			: m_ColliderEntityID(colliderID)
		{
		}

		static bool Compare(Collision* a, Collision* b);
	};

	/**
	 * Used for Project specific Physics configurations
	 */
	struct PhysicsConfig
	{
		glm::vec2 m_GravityForce = { 0.0f, -9.8f }; 
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

		static Collision* FindCollision(uint32_t ownerID, uint32_t otherID);
	};
}