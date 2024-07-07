#include "stpch.h"
#include "RuntimeApplication.h"
#include "RuntimeLayer.h"

#include "Stimpi/Scene/ResourceManager.h"

namespace Stimpi
{
	Application* CreateApplication()
	{
		// Load project data
		Stimpi::ResourceManager::LoadDefaultProject();

		return new RuntimeApplication();
	}

	RuntimeApplication::RuntimeApplication()
	{
		PushOverlay(new RuntimeLayer());
	}

	RuntimeApplication::~RuntimeApplication()
	{

	}

}