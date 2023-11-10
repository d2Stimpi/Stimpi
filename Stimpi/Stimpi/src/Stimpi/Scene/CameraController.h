#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Core/Timestep.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Scene/Camera.h"

namespace Stimpi
{
	class ST_API CameraController
	{
	public:
		CameraController(Camera* camera);
		~CameraController();

		void AttachCamera(Camera* camera);
		void Update(Timestep ts);

		void SetMouseControllesActive(bool active) { m_MouseActive = active; }

	private:
		Camera* m_Camera;
		std::shared_ptr<MouseEvnetHandler> m_MouseHandler;
		bool m_MouseActive;
	};
}