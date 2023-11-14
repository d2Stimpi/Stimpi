#include "stpch.h"
#include "Stimpi/Graphics/OrthoCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stimpi
{
	OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
		: m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_ViewMatrix(1.0f), m_Positin(0.0f)
	{
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
		// Serialization data
		m_ViewQuad = glm::vec4(left, right, bottom, top);
	}

	void OrthoCamera::Translate(glm::vec3 vector)
	{
		m_Positin += vector;
		RecalcViewProjection();
	}

	void OrthoCamera::Resize(float left, float right, float bottom, float top)
	{
		m_ViewQuad = glm::vec4(left, right, bottom, top);
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		RecalcViewProjection();
	}

	void OrthoCamera::SetOrtho(glm::vec4 view)
	{
		Resize(view.x, view.y, view.z, view.w);
	}

	void OrthoCamera::RecalcViewProjection()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Positin) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.f, 0.f, 1.f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(m_Zoom, m_Zoom, 1.0f));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}