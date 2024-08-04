#include <gtest/gtest.h>

#include "Stimpi/Log.h"

#include "Stimpi/Graphics/OpenGL.h"
#include "Stimpi/Core/WindowManager.h"

std::unique_ptr<Stimpi::Window> g_Window;
Stimpi::GLContext g_Context;

int main(int argc, char** argv)
{
	Stimpi::Log::Init();
	Stimpi::Log::Disable();

	// Create "dummy" window and initialize SDL and OpenGL
	g_Window.reset(Stimpi::Window::CreateAppWindow());
	g_Context.CreateContext(g_Window.get());

	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}