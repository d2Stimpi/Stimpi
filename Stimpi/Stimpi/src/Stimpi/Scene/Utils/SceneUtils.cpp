#include "stpch.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"

#include "Stimpi/Log.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stimpi
{

	glm::vec2 SceneUtils::WindowToWorldPoint(Camera* camera, glm::vec2 windowSize, glm::vec2 winPos)
	{
		if (camera == nullptr)
			return { 0.0f, 0.0f };

		glm::vec3 cameraPosition = camera->GetPosition();
		glm::vec4 cameraBounds = camera->GetOrthoView();
		float cameraZoom = camera->GetZoomFactor();
		float cameraAspect = camera->GetAspectRatio();

		// From window size and aspect ration we infer framebuffer size
		glm::vec2 fbSize;
		if (windowSize.x / cameraAspect >= windowSize.y)
		{
			fbSize.x = (uint32_t)windowSize.x;
			fbSize.y = (uint32_t)(windowSize.x / cameraAspect);
		}
		else
		{
			fbSize.x = (uint32_t)(windowSize.y * cameraAspect);
			fbSize.y = (uint32_t)windowSize.y;
		}
		// Calculate visible camera bounds. Not full FrameBuffer is always visible
		glm::vec2 cameraVisibleSize = { cameraBounds.y * windowSize.x / fbSize.x, cameraBounds.w * windowSize.y / fbSize.y };

		// Adjusted camera view and window size ratio
		glm::vec2 scale = { windowSize.x / cameraVisibleSize.x, windowSize.y / cameraVisibleSize.y };

		winPos = winPos * cameraZoom;
		glm::vec2 worldPos = { winPos.x / scale.x + cameraPosition.x, winPos.y / scale.y + cameraPosition.y };

		return worldPos;
	}

	glm::vec2 SceneUtils::WorldToWindowPoint(Camera* camera, glm::vec2 windowSize, glm::vec2 worldPos)
	{
		if (camera == nullptr)
			return { 0.0f, 0.0f };

		glm::vec3 cameraPosition = camera->GetPosition();
		glm::vec4 cameraBounds = camera->GetOrthoView();
		float cameraZoom = camera->GetZoomFactor();
		float cameraAspect = camera->GetAspectRatio();

		// From window size and aspect ration we infer framebuffer size
		glm::vec2 fbSize;
		if (windowSize.x / cameraAspect >= windowSize.y)
		{
			fbSize.x = (uint32_t)windowSize.x;
			fbSize.y = (uint32_t)(windowSize.x / cameraAspect);
		}
		else
		{
			fbSize.x = (uint32_t)(windowSize.y * cameraAspect);
			fbSize.y = (uint32_t)windowSize.y;
		}
		// Calculate visible camera bounds. Not full FrameBuffer is always visible
		glm::vec2 cameraVisibleSize = { cameraBounds.y * windowSize.x / fbSize.x, cameraBounds.w * windowSize.y / fbSize.y };

		// Adjusted camera view and window size ratio
		glm::vec2 scale = { windowSize.x / cameraVisibleSize.x, windowSize.y / cameraVisibleSize.y };

		worldPos = worldPos - glm::vec2{ cameraPosition.x, cameraPosition.y };
		worldPos = worldPos / cameraZoom;
		glm::vec2 winPos = { worldPos.x * scale.x, worldPos.y * scale.y };

		return winPos;
	}

	bool SceneUtils::IsContainedInSquare(glm::vec2 point, glm::vec2 min, glm::vec2 max)
	{
		if ((point.x >= min.x) && (point.x <= max.x) &&
			(point.y >= min.y) && (point.y <= max.y))
		{
			return true;
		}

		return false;
	}

	bool SceneUtils::IsPointInRotatedSquare(glm::vec2 point, glm::vec2 center, glm::vec2 size, float rotation)
	{
		glm::mat4 transform = glm::rotate(glm::mat4(1.0f), -rotation, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec4 vectorPointCenter = { point.x - center.x, point.y - center.y, 1.0f, 1.0f };
		glm::vec4 rotatedVector = transform * vectorPointCenter;

		if ((rotatedVector.x > -size.x / 2.0f) && (rotatedVector.x < size.x / 2.0f) &&
			(rotatedVector.y > -size.y / 2.0f) && (rotatedVector.y < size.y / 2.0f))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool SceneUtils::IsPointInCircle(glm::vec2 point, glm::vec2 center, float radius)
	{
		float distancePointCenter = glm::distance(point, center);

		if (distancePointCenter <= radius)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}