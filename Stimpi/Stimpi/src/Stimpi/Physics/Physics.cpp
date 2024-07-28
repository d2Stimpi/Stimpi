#include "stpch.h"
#include "Stimpi/Physics/Physics.h"

namespace Stimpi
{
	bool Collision::Compare(Collision* a, Collision* b)
	{
		return (a->m_Owner == b->m_Owner) && (a->m_ColliderEntityID == b->m_ColliderEntityID);
	}

	struct PhysicsDebugConfig
	{
		// Debugging
		bool m_ShowDbgColliderOutline = false;
		bool m_ShowDbgCollisionContactPoints = false;
	};

	PhysicsDebugConfig s_Data;

	std::vector<std::shared_ptr<Collision>> m_ActiveCollisions;
	std::shared_ptr<Collision> m_ActiveCollision;	//TODO: consider threading here, and a global Collision ID. 
													// With the ID we can collect every collision in the frame
													// and have them all available during full frame duration.

	void Physics::ShowColliderOutline(bool enable)
	{
		s_Data.m_ShowDbgColliderOutline = enable;
	}

	bool Physics::ShowColliderOutlineEnabled()
	{
		return s_Data.m_ShowDbgColliderOutline;
	}

	void Physics::ShowCollisionsContactPoints(bool enable)
	{
		s_Data.m_ShowDbgCollisionContactPoints = enable;
	}

	bool Physics::ShowCollisionsContactPointsEnabled()
	{
		return s_Data.m_ShowDbgCollisionContactPoints;
	}


	void Physics::SetActiveCollision(Collision* collision)
	{
		// Either clear the data or replace the data
		if (collision != nullptr)
			m_ActiveCollision.reset(collision);
		else
			m_ActiveCollision.reset();
	}


	Stimpi::Collision* Physics::GetActiveCollision()
	{
		return m_ActiveCollision.get();
	}

	void Physics::AddActiveCollision(Collision* collision)
	{
		if (collision != nullptr)
		{
			std::shared_ptr<Collision> data;
			data.reset(collision);
			m_ActiveCollisions.push_back(data);
		}
	}

	void Physics::RemoveActiveCollision(Collision* collision)
	{
		if (collision != nullptr)
		{
			m_ActiveCollisions.erase(std::remove_if(m_ActiveCollisions.begin(), m_ActiveCollisions.end(),
				[&collision](std::shared_ptr<Collision> item)
				{
					return Collision::Compare(item.get(), collision);
				}));
		}
	}

	void Physics::UpdateActiveCollision(Collision* collision)
	{
		if (collision != nullptr)
		{
			if (auto it = std::find_if(m_ActiveCollisions.begin(), m_ActiveCollisions.end(),
				[&collision](std::shared_ptr<Collision> item)
				{
					return Collision::Compare(item.get(), collision);
				}
			); it != m_ActiveCollisions.end())
			{
				auto foundCollision = *it;
				foundCollision->m_Contacts = collision->m_Contacts;
			}
		}
	}

	void Physics::ClearActiveCollisions()
	{
		if (!m_ActiveCollisions.empty())
			m_ActiveCollisions.clear();
	}

	std::vector<std::shared_ptr<Stimpi::Collision>>& Physics::GetActiveCollisions()
	{
		return m_ActiveCollisions;
	}

	Collision* Physics::FindCollision(uint32_t ownerID, uint32_t otherID)
	{
		for (auto& collision : m_ActiveCollisions)
		{
			if ((collision->m_Owner == ownerID) && (collision->m_ColliderEntityID == otherID))
			{
				return collision.get();
			}
		}
	}

}