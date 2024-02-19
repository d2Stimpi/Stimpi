#include "stpch.h"
#include "Gui/Nodes/NodePanel.h"

#include "Stimpi/Log.h"

#include "ImGui/src/imgui_internal.h"

namespace Stimpi
{
	bool s_Show = false;

	NodePanel::NodePanel()
	{
		m_HeaderImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/Gradient2.png");
		//m_HeaderImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/sovingradient.png");
	}

	NodePanel::~NodePanel()
	{
		AssetManager::Release(m_HeaderImage);
	}

	void NodePanel::OnImGuiRender()
	{
		if (s_Show)
		{
			ImGui::Begin("Test Node Panel", &s_Show);

			static float scale = 1.0f;
			ImGui::DragFloat("Panel scale", &scale, 0.01f, 0.02f, 10.0f);

			static ImVec2 scrolling(0.0f, 0.0f);
			ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
			if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
			if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
			ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

			// Draw border and background color
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			auto drawListStartVertexIndex = draw_list->_VtxCurrentIdx;
			auto drawListFirstCommandIndex = ImMax(draw_list->CmdBuffer.Size - 1, 0);
			ImVec2 viewTransformPosition = { 0.0f, 0.0f };//ImGui::GetCursorScreenPos();


			draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
			draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

			// This will catch our interactions
			ImGuiIO& io = ImGui::GetIO();
			ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool is_hovered = ImGui::IsItemHovered(); // Hovered
			const bool is_active = ImGui::IsItemActive();   // Held

			if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
			{
				scrolling.x += io.MouseDelta.x;
				scrolling.y += io.MouseDelta.y;
			}
			//ImGui::IsMouseHoveringRect()

			ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // scrolled origin
			const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

			// Draw grid + all lines in the canvas
			draw_list->PushClipRect(canvas_p0, canvas_p1, true);
			{
				const float GRID_STEP = 64.0f * scale;
				for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
					draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
				for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
					draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
			}
			// Draw node frame
			draw_list->AddRectFilled({ origin.x + 20.0f * scale, origin.y + 20.0f * scale }, { origin.x + 300.0f * scale, origin.y + 150.0f * scale }, IM_COL32(120, 120, 120, 255), 15.0f * scale);
			
			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			auto texture = AssetManager::GetAsset(m_HeaderImage).As<Texture>();
			if (texture->Loaded())
			{
				draw_list->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
					{ origin.x + 20.0f * scale, origin.y + 20.0f * scale }, { origin.x + 300.0f * scale, origin.y + 50.0f * scale },
					uv_min, uv_max, IM_COL32(225, 30, 30, 255), 15 * scale, ImDrawFlags_RoundCornersTop);
			}

			/*ImU32 col_a = ImGui::GetColorU32(IM_COL32(200, 30, 30, 255));
			ImU32 col_b = ImGui::GetColorU32(IM_COL32(30, 30, 30, 255));
			draw_list->AddRectFilledMultiColor({ origin.x + 22.0f * scale, origin.y + 22.0f * scale }, { origin.x + 198.0f * scale, origin.y + 40.0f * scale }, col_a, col_b, col_b, col_a);
			*/
			ImGui::SetWindowFontScale(scale);
			draw_list->AddText({ origin.x + 45.0f * scale, origin.y + 20.0f * scale }, IM_COL32(255, 255, 255, 255), "Node name");
			ImGui::SetWindowFontScale(1.0f);
			
			//draw_list->ChannelsSetCurrent()
			
			draw_list->PopClipRect();



			ImGui::End();
		}
	}

	void NodePanel::ShowWindow(bool show)
	{
		s_Show = show;
	}

	bool NodePanel::IsVisible()
	{
		return s_Show;
	}

}