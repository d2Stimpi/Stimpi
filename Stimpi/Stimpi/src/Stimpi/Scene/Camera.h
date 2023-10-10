#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/OrthoCamera.h"

/* Only Orthographic Camera type is supported */
/* TODO: impl aspect ratio handling */

namespace Stimpi
{
	class ST_API Camera
	{
	public:
		Camera(float left, float right, float bottom, float top);
		~Camera();

		void Translate(glm::vec3 vector) { m_Camera->Translate(vector); }
		void Resize(float left, float right, float bottom, float top) { m_Camera->Resize(left, right, bottom, top); }

		void SetPosition(glm::vec3 position) { m_Camera->SetPosition(position); }
		const glm::vec3& GetPosition() const { return m_Camera->GetPosition(); }

		void SetAspectRation(float width, float height) { m_AspectRatio = width / height; }
		float GetAspectRatio() { return m_AspectRatio; }

		const glm::mat4& GetProjectionMatrix() const { return m_Camera->GetProjectionMatrix(); }
		const glm::mat4& GetViewMatrix() const { return m_Camera->GetViewMatrix(); }
		const glm::mat4& GetViewProjectionMatrix() const { return m_Camera->GetViewProjectionMatrix(); }

		OrthoCamera* GetOrthoCamera() { return m_Camera.get(); }

	private:
		std::shared_ptr<OrthoCamera> m_Camera;
		float m_AspectRatio;
	};
}