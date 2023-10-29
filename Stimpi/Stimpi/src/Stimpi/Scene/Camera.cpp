#include "stpch.h"
#include "Stimpi/Scene/Camera.h"

namespace Stimpi
{
	Camera::Camera()
		: Camera(0.0f, 1280.0f, 0.0f, 720.0f)
	{

	}

	Camera::Camera(float left, float right, float bottom, float top)
	{
		m_Camera = std::make_shared<OrthoCamera>(left, right, bottom, top);
		SetAspectRation(right, top);
		
		m_ViewportWidth = right;
		m_ViewportHeight = top;
		m_ZoomFactor = 1.0f;
	}

	Camera::~Camera()
	{

	}

	void Camera::SetZoomFactor(float zoom)
	{
		m_ZoomFactor = zoom;

		// TODO: reconsider doing this here, a bit weird (since OrthoCam does not hold AspectRatio info)

		float width = m_ViewportWidth * zoom;
		float height = m_ViewportHeight * zoom;

		if (width / m_AspectRatio > height)
			Resize(0.0f, width, 0.0f, width / m_AspectRatio);
		else
			Resize(0.0f, height * m_AspectRatio, 0.0f, height);
	}

}