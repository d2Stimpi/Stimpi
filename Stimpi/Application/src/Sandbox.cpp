#include "Stimpi.h"

class TestApp : public Stimpi::Application
{
public:
	TestApp()
	{

	}

	~TestApp()
	{

	}
};

Stimpi::Application* Stimpi::CreateApplication()
{
	return new TestApp();
}