#include "stpch.h"
#include "Stimpi/Gui/SceneViewWindow.h"

#include "Stimpi/Graphics/Renderer2D.h"

namespace Stimpi
{

	SceneViewWindow::SceneViewWindow()
	{
		m_Flags = ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoCollapse;
	}

	SceneViewWindow::~SceneViewWindow()
	{

	}

	void SceneViewWindow::Draw()
	{
		auto frameBuffer = Renderer2D::Instace()->GetFrameBuffer();

		ImGui::Begin("OpenGL Main Scene View", &m_Show, m_Flags);

		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImVec2 ws = ImGui::GetContentRegionAvail();
		// Invert the image on ImGui window
		ImVec2 uv_min = ImVec2(0.0f, ws.y / frameBuffer->GetHeight());
		ImVec2 uv_max = ImVec2(ws.x / frameBuffer->GetWidth(), 0.0f);

		// Aspect ratio 16:9
		/*if (((float)ws.x / 1.7778f) >= (float)ws.y)
		{
			frameBuffer->Resize(ws.x, ((float)ws.x / 1.7778f));
		}
		else
		{
			frameBuffer->Resize(((float)ws.y*1.778f), ws.y);
		}*/

		ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(pos), ImVec2(pos.x + ws.x, pos.y + ws.y), uv_min, uv_max);
		ImGui::End();
	}

}