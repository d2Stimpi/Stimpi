#include "Stimpi/Graphics/Camera.h"

#include <glm/gtc/type_ptr.hpp>

namespace Stimpi
{
	Camera::Camera(float sceneWidth, float sceneHeight)
		: m_SceneWidth(sceneWidth), m_SceneHeihgt(sceneHeight)
	{
		m_Projection = glm::ortho(0.0f, m_SceneWidth, 0.0f, m_SceneHeihgt, -1.0f, 1.0f);
		m_Model = glm::mat4(1.0f);
		m_Mvp = m_Projection * m_Model;
	}

	Camera::~Camera()
	{

	}

	void Camera::BindToShader(Shader& shader)
	{
		shader.Use();
		shader.SetUniform("mvp", m_Mvp);
	}
}