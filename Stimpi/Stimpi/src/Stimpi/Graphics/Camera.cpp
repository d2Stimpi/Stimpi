#include "Stimpi/Graphics/Camera.h"

#include <glm/gtc/type_ptr.hpp>

#include "Stimpi/Log.h"

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
		ST_CORE_INFO("Camera::~Camera() called");
	}

	void Camera::BindToShader(Shader& shader)
	{
		// Update ModelViewProj
		m_Mvp = m_Projection * m_Model;

		shader.Use();
		shader.SetUniform("mvp", m_Mvp);
	}

	void Camera::Translate(glm::vec3 vector)
	{
		// Translate with invers matrix to "move" the scene (ignore z for now)
		m_Model = glm::translate(m_Model, glm::vec3(-vector.x, -vector.y, vector.z));
	}

	void Camera::Zoom(float factor)
	{
		if (factor <= 0)
		{
			ST_CORE_WARN("Camera - zoom factor is invalid!");
			return;
		}
		// TODO: check for better way
		m_Projection = glm::ortho(0.0f, factor*m_SceneWidth, 0.0f, factor*m_SceneHeihgt, -1.0f, 1.0f);
	}

	void Camera::Resize(float sceneWidth, float sceneHeight)
	{
		m_SceneWidth = sceneWidth;
		m_SceneHeihgt = sceneHeight;

		m_Projection = glm::ortho(0.0f, m_SceneWidth, 0.0f, m_SceneHeihgt, -1.0f, 1.0f);
	}
}