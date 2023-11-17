#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Camera.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	class ST_API SceneUtils
	{
	public:
		static glm::vec2 WindowToWorldPoint(Camera* camera, glm::vec2 windowSize, glm::vec2 winPos);
		static glm::vec2 WorldToWindowPoint(Camera* camera, glm::vec2 windowSize, glm::vec2 worldPos);
	};
}