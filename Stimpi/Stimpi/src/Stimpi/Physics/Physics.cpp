#include "stpch.h"
#include "Stimpi/Physics/Physics.h"

namespace Stimpi
{
	struct PhysicsConfig
	{
		bool m_ShowDbgColliderOutline = false;
	};

	PhysicsConfig s_Data;

	void Physics::ShowColliderOutline(bool enable)
	{
		s_Data.m_ShowDbgColliderOutline = enable;
	}

	bool Physics::ShowColliderOutlineEnabled()
	{
		return s_Data.m_ShowDbgColliderOutline;
	}

}