#pragma once

#include <glm/glm.hpp>

#include "Stimpi/Core/Core.h"
#include "Stimpi/Graphics/Shader.h"

namespace Stimpi
{
	class ST_API Camera
	{
	public:
		Camera(float sceneWidth, float sceneHeight);
		~Camera();

		void Translate(glm::vec3 vector);
		void Zoom(float factor);
		void Resize(float sceneWidth, float sceneHeight);

		glm::mat4 GetMvpMatrix() { return m_Mvp; }
		glm::mat4 GetProjectionMatrix() { return m_Projection; }
		glm::mat4 GetModelMatrix() { return m_Model; }
		glm::mat4 GetViewMatrix() { return m_View; }

	private:
		void UpdateMvp();

		float m_SceneWidth;
		float m_SceneHeihgt;
		glm::mat4 m_Projection; // Ortho
		glm::mat4 m_Model; // M = T(transl) * R(rot) * S(scale)
		glm::mat4 m_View;
		glm::mat4 m_Mvp; // Model * View * Proj
	};
}