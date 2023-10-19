#pragma once

#include "Stimpi/Core/Core.h"

#include <glm/glm.hpp>

namespace Stimpi
{
	class ST_API OrthoCamera
	{
	public:
		OrthoCamera(float left, float right, float bottom, float top);

		void Translate(glm::vec3 vector);
		void Resize(float left, float right, float bottom, float top);

		const glm::vec3& GetPosition() const { return m_Positin; }
		void SetPosition(glm::vec3 position) { m_Positin = position; RecalcViewProjection(); }

		float GetRotation() { return m_Rotation; }
		void SetRotation(float rotation) { m_Rotation = rotation; RecalcViewProjection(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		// Serialization use
		glm::vec4 GetViewQuad() { return m_ViewQuad; }

	private:
		void RecalcViewProjection();

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Positin;
		float m_Rotation = 0.0f;

		// Serialization use
		glm::vec4 m_ViewQuad;
	};
}