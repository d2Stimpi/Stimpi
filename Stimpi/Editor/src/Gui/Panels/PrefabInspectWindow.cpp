#include "stpch.h"
#include "Gui/Panels/PrefabInspectWindow.h"

#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/Camera.h"

namespace Stimpi
{
	bool PrefabInspectWindow::m_Show = false;

	struct PrefabInspectWindowContext
	{
		std::shared_ptr<Camera> m_Camera{};
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
	};

	PrefabInspectWindowContext s_Context;

	PrefabInspectWindow::PrefabInspectWindow()
	{
		s_Context.m_Camera = std::make_shared<Camera>(0.0f, 128.0f, 0.0f, 72.0f);
		s_Context.m_Camera->SetPosition({ 0.0f, 0.0f, 0.0f });
		s_Context.m_FrameBuffer.reset(Renderer2D::Instance()->CreateFrameBuffer(0, 0, DEFAULT_COLOR_CHANNEL_NUMBER));
	}

	PrefabInspectWindow::~PrefabInspectWindow()
	{

	}

	void PrefabInspectWindow::OnImGuiRender()
	{
		if (m_Show)
		{
			auto& frameBuffer = s_Context.m_FrameBuffer;

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
			ImGui::Begin("PrefabInspector##PrefabInspectWindow", &m_Show);
			ImGui::PopStyleVar();

			ImVec2 pos = ImGui::GetCursorScreenPos();
			ImVec2 winSize = ImGui::GetContentRegionAvail();
			// Invert the image on ImGui window
			ImVec2 uv_min = ImVec2(0.0f, winSize.y / frameBuffer->GetHeight());
			ImVec2 uv_max = ImVec2(winSize.x / frameBuffer->GetWidth(), 0.0f);

			// Aspect ratio 16:9
			// TODO: configurable aspect ratio
			if (((float)winSize.x / 1.7778f) >= (float)winSize.y)
			{
				frameBuffer->Resize(winSize.x, std::round((float)winSize.x / 1.7778f));
			}
			else
			{
				frameBuffer->Resize(std::round((float)winSize.y * 1.778f), winSize.y);
			}
			// Update the camera position and size so that the 0.0.0 is the window center
			glm::vec3 cameraPos = { winSize.x / -2.0f, winSize.y / -2.0f, 0.0f };
			s_Context.m_Camera->Resize(0.0f, frameBuffer->GetWidth(), 0.0f, frameBuffer->GetHeight());
			s_Context.m_Camera->SetPosition(cameraPos);

			ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(pos), ImVec2(pos.x + winSize.x, pos.y + winSize.y), uv_min, uv_max);

			// Draw on the PrefabInspector's FrameBuffer
			auto renderer = Renderer2D::Instance();
			renderer->StartFrame(frameBuffer.get());
			{
				// Draw PrefabInspector Scene data
				renderer->BeginScene(s_Context.m_Camera->GetOrthoCamera());
				Renderer2D::Instance()->SubmitCircle({ 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f }, { 0.8f, 0.8f, 0.2f, 1.0f }, 1.0f, 0.005f);
				renderer->EndScene();
			}
			renderer->EndFrame(frameBuffer.get());


			/*ImGui::Text("Window Size: %f, %f", winSize.x, winSize.y);
			ImGui::Text("FB Size: %d, %d", frameBuffer->GetWidth(), frameBuffer->GetHeight());
			ImGui::Text("Camera Position: %f, %f", cameraPos.x, cameraPos.y);*/

			ImGui::End();
		}
	}

	void PrefabInspectWindow::ShowWindow(bool show)
	{
		m_Show = show;
	}

	bool PrefabInspectWindow::IsVisible()
	{
		return m_Show;
	}

}