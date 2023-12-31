#pragma once

#include "Stimpi/Scene/Scene.h"

#include "box2d/b2_world_callbacks.h"

namespace Stimpi
{
	class ContactListener : public b2ContactListener
	{
	public:
		void SetContext(Scene* scene);

		void BeginContact(b2Contact* contact);
		void EndContact(b2Contact* contact);
	private:
		Scene* m_Scene;
	};
}