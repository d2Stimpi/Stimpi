#include "stpch.h"
#include "Gui/Config/EditorConfig.h"

#include "Stimpi/Core/Time.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	
	EditorConfig::EditorConfig()
	{

	}

	EditorConfig::~EditorConfig()
	{

	}

	void EditorConfig::OnImGuiRender()
	{
		if (m_Show)
		{
			ImGui::Begin("Global Stats", &m_Show);
			ImGui::Text("Application FPS %.1f", Time::Instance()->GetActiveFPS());
			ImGui::Text("Average %.1f ms/frame", 1000.0f / Time::Instance()->GetActiveFPS());

			ImGui::End();
		}
	}

}