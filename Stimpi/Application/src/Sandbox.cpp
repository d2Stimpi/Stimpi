#include "Stimpi.h"

class TestApp : public Stimpi::Application
{
public:
	TestApp()
	{

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