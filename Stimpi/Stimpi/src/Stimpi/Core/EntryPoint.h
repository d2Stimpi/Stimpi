#include "stpch.h"
#pragma once

#ifndef STIMPI_BUILD_DLL

#ifndef ENTRY_DEF
#define ENTRY_DEF

extern Stimpi::Application* Stimpi::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Stimpi::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#endif

#endif