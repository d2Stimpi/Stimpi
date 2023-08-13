#include "Stimpi.h"

class TestLayer : public Stimpi::Layer
{
public:
	TestLayer() : Layer("TestLayer") {}

	void OnAttach() override { ST_TRACE("{0}: OnAttach", m_DebugName); }
	void OnDetach() override { ST_TRACE("{0}: OnDetach", m_DebugName); }
	void Update() override { /*ST_TRACE("{0}: Update", m_DebugName); */}
	void OnEvent(Stimpi::Event e) override { /*ST_TRACE("{0}: OnEvent", m_DebugName);*/ }
};

class TestApp : public Stimpi::Application
{
public:
	TestApp()
	{
		PushLayer(new TestLayer());
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