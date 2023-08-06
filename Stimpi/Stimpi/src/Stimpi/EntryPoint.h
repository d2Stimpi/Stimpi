#pragma once

#ifndef STIMPI_BUILD_DLL

extern Stimpi::Application* Stimpi::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Stimpi::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif