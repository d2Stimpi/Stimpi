#include "stpch.h"
#include "Stimpi/Scene/Utils/SceneUtils.h"

#include "Stimpi/Log.h"

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
		
		//ST_CORE_INFO("   WindowToWorldPoint: cam {0}, {1}", cameraVisibleSize.x, cameraVisibleSize.y);
		//ST_CORE_INFO("   WindowToWorldPoint: fb {0}, {1}", fbSize.x, fbSize.y);

		// Adjusted camera view and window size ratio
		glm::vec2 scale = { windowSize.x / cameraVisibleSize.x, windowSize.y / cameraVisibleSize.y };
		//ST_CORE_INFO("   WindowToWorldPoint: scale {0}, {1}", scale.x, scale.y);

		winPos = winPos * cameraZoom;
		glm::vec2 worldPos = { winPos.x / scale.x + cameraPosition.x, winPos.y / scale.y + cameraPosition.y };
		ST_CORE_INFO("   WindowToWorldPoint: pos {0}, {1}", worldPos.x , worldPos.y);

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

}