#include "stpch.h"
#pragma once

#ifndef STIMPI_BUILD_DLL

#ifndef ENTRY_DEF
#define ENTRY_DEF

extern Stimpi::Application* Stimpi::CreateApplication();

int main(int argc, char** argv)
{
	Stimpi::Log::Init();
	ST_PROFILE_BEGIN_SESSION("Create Application", "create_app.json");
	auto app = Stimpi::CreateApplication();
	{
		ST_PROFILE_SCOPE("Test");
	}
	
	ST_PROFILE_END_SESSION();

	ST_PROFILE_BEGIN_SESSION("Run Application", "run_app.json");
	app->Run();
	ST_PROFILE_END_SESSION();

	ST_PROFILE_BEGIN_SESSION("End Application", "end_app.json");
	delete app;
	ST_PROFILE_END_SESSION();
	return 0;
}

#endif

#endif