#include <gtest/gtest.h>

#include "Stimpi/Log.h"

#include "Stimpi/Graphics/OpenGL.h"
#include "Stimpi/Core/WindowManager.h"
#include "Stimpi/Scripting/ScriptEngine.h"
#include "Stimpi/Scene/ResourceManager.h"

std::unique_ptr<Stimpi::Window> g_Window;
Stimpi::GLContext g_Context;

int main(int argc, char** argv)
{
	Stimpi::Log::Init();
	Stimpi::Log::Disable();

	// Create "dummy" window and initialize SDL and OpenGL
	g_Window.reset(Stimpi::Window::CreateAppWindow());
	g_Context.CreateContext(g_Window.get());

	Stimpi::ResourceManager::LoadDefaultProject();
	Stimpi::ScriptEngine::Init();

	testing::InitGoogleTest(&argc, argv);

	RUN_ALL_TESTS();

	Stimpi::ScriptEngine::Shutdown();

	return 1;
}