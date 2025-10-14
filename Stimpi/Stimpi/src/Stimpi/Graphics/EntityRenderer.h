#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Camera.h"

#include <entt/entt.hpp>

namespace Stimpi
{
	class ST_API EntityRenderer
	{
	public:
		EntityRenderer() {};
		~EntityRenderer() {};
		
		void SetCamera(Camera* camera) { m_RenderCamera = camera; };
		void SubmitForRendering(entt::registry& registry);

	private:
		Camera* m_RenderCamera = nullptr;
	};
}