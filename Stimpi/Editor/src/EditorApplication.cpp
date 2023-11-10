#include "Stimpi.h"
#include "Stimpi/Core/EntryPoint.h"

#include "Gui/EditorLayer.h"

class EditorApp : public Stimpi::Application
{
public:
	EditorApp()
	{
		PushOverlay(new Stimpi::EditorLayer(m_Window.get(), &m_Context.GetContext()));
	}

	~EditorApp()
	{
		ST_INFO("~EditorApp()");
	}

private:
};

Stimpi::Application* Stimpi::CreateApplication()
{
	return new EditorApp();
}