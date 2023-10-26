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
	}

	Camera::~Camera()
	{

	}
}