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

		void BindToShader(Shader& shader);

		void Translate(glm::vec3 vector);
		void Zoom(float factor);
		void Resize(float sceneWidth, float sceneHeight);

		glm::mat4 GetMvpMatrix() { return m_Mvp; }

	private:
		float m_SceneWidth;
		float m_SceneHeihgt;
		glm::mat4 m_Projection; // Ortho
		glm::mat4 m_Model; // M = T(transl) * R(rot) * S(scale)
		glm::mat4 m_Mvp; // Model * View * Proj
	};
}