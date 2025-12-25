#include "stpch.h"
#include "Gui/Panels/PrefabInspectWindow.h"

#include "Stimpi/Asset/PrefabManager.h"
#include "Stimpi/Asset/ShaderImporter.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/Graphics/Renderer2D.h"
#include "Stimpi/Scene/Scene.h"
#include "Stimpi/Scene/Entity.h"
#include "Stimpi/Scene/Camera.h"

#include "Gui/Components/Toolbar.h"
#include "Gui/Gizmo2D.h"

namespace Stimpi
{
	bool PrefabInspectWindow::m_Show = false;

	struct PrefabInspectWindowContext
	{
		std::shared_ptr<Scene> m_Scene{};
		Entity m_PrefabEntity{};
		std::shared_ptr<Camera> m_Camera{};
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
	};

	PrefabInspectWindowContext s_Context;

	PrefabInspectWindow::PrefabInspectWindow()
	{
		s_Context.m_Camera = std::make_shared<Camera>(0.0f, 128.0f, 0.0f, 72.0f);
		s_Context.m_Camera->SetPosition({ 0.0f, 0.0f, 0.0f });
		s_Context.m_FrameBuffer.reset(Renderer2D::Instance()->CreateFrameBuffer(0, 0, DEFAULT_COLOR_CHANNEL_NUMBER));

		s_Context.m_Scene = std::make_shared<Scene>();
		s_Context.m_Scene->SetCamera(s_Context.m_Camera.get());
		s_Context.m_Scene->SetScriptingEnabled(false);
	}

	PrefabInspectWindow::~PrefabInspectWindow()
	{

	}

	void PrefabInspectWindow::OnImGuiRender(Timestep ts)
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
			glm::vec2 fbSize = { frameBuffer->GetWidth(), frameBuffer->GetHeight() };
			glm::vec3 cameraPos = { winSize.x / -20.0f, winSize.y / -20.0f, 0.0f };
			s_Context.m_Camera->Resize(0.0f, frameBuffer->GetWidth() / 10.0f, 0.0f, frameBuffer->GetHeight() / 10.0f);
			s_Context.m_Camera->SetPosition(cameraPos);
			s_Context.m_Camera->Translate({ 0.0f, 0.0f, 0.0f });
;
			ImGui::GetWindowDrawList()->AddImage((void*)(intptr_t)frameBuffer->GetTextureID(), ImVec2(pos), ImVec2(pos.x + winSize.x, pos.y + winSize.y), uv_min, uv_max);

			// Custom Rendering stuff
			auto canvasWidth = Renderer2D::Instance()->GetCanvasWidth();
			auto canvasHeight = Renderer2D::Instance()->GetCanvasHeight();

			// Draw on the PrefabInspector's FrameBuffer
			auto renderer = Renderer2D::Instance();
			renderer->StartFrame(frameBuffer.get());
			{
				// Draw PrefabInspector Scene data
				s_Context.m_Scene->OnUpdate(ts);
			}
			renderer->EndFrame(frameBuffer.get());


			/*ImGui::Text("Window Size: %f, %f", winSize.x, winSize.y);
			ImGui::Text("FB Size: %d, %d", frameBuffer->GetWidth(), frameBuffer->GetHeight());
			ImGui::Text("Camera Position: %f, %f", cameraPos.x, cameraPos.y);
			*/
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

	void PrefabInspectWindow::SetPrefabEntity(AssetHandle prefabHandle)
	{
		s_Context.m_Scene->RemoveAllEntites();
		s_Context.m_PrefabEntity = PrefabManager::InstantiatePrefab(s_Context.m_Scene.get(), prefabHandle, {0.0f, 0.0f, 0.0f});
	}

	Stimpi::Entity PrefabInspectWindow::GetPrefabEntity()
	{
		return s_Context.m_PrefabEntity;
	}

	Stimpi::Scene* PrefabInspectWindow::GetScene()
	{
		return s_Context.m_Scene.get();
	}

}