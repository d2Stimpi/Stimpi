#include "Stimpi.h"
// Entry point - include only once
#include "Stimpi/Core/EntryPoint.h"

#include "OpenGLTestLayer.h"

class TestApp : public Stimpi::Application
{
public:
	TestApp()
	{
		PushLayer(new OpenGLTestLayer());
	}

	~TestApp()
	{
		ST_TRACE("~TestApp()");
	}
};

Stimpi::Application* Stimpi::CreateApplication()
{
	return new TestApp();
}