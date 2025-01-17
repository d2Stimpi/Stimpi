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
		// Flips Y axis
		static glm::vec2 WindowToScenePosition(glm::vec2 windowSize, glm::vec2 screenPos);
		static glm::vec2 SquareBoundPosition(glm::vec2 windowSize, glm::vec2 screnePos);

		static bool IsContainedInSquare(glm::vec2 point, glm::vec2 min, glm::vec2 max);
		static bool IsPointInRotatedSquare(glm::vec2 point, glm::vec2 center, glm::vec2 size, float rotation);
		static bool IsPointInCircle(glm::vec2 point, glm::vec2 center, float radius);
	};
}