#include "stpch.h"
#include "Gui/NNode/NGraphRenderer.h"

#include "Gui/NNode/NGraphPanel.h"
#include "Gui/NNode/NGraphStyle.h"

#include "Stimpi/Asset/TextureImporter.h"
#include "Stimpi/Asset/AssetManager.h"
#include "Stimpi/Core/Project.h"

namespace Stimpi
{

	struct NGraphRendererContext
	{
		ImDrawList* m_DrawList = nullptr;
		NGraphPanel* m_PanelContext = nullptr;
		NGraphPanelCanvas* m_Canvas = nullptr;

		bool m_DebugOn = false;

		std::shared_ptr<Texture> m_HeaderImage;
		std::shared_ptr<Texture> m_HighlightImage;
	};

	NGraphRendererContext s_Context;

	void NGraphRenderer::SetPanelContext(NGraphPanel* panelContext)
	{
		s_Context.m_PanelContext = panelContext;
		s_Context.m_HeaderImage = TextureImporter::LoadTexture(Project::GetAssestsDir() / "textures\/Gradient2.png");
		s_Context.m_HighlightImage = TextureImporter::LoadTexture(Project::GetAssestsDir() / "textures\/Highlight.png");
	}

	void NGraphRenderer::SetDrawList(ImDrawList* drawList)
	{
		s_Context.m_DrawList = drawList;
	}

	void NGraphRenderer::SetDrawCanvas(NGraphPanelCanvas* canvas)
	{
		s_Context.m_Canvas = canvas;
	}

	ImDrawList* NGraphRenderer::GetDrawList()
	{
		return s_Context.m_DrawList;
	}

	NGraphPanelCanvas* NGraphRenderer::GetDrawCanvas()
	{
		return s_Context.m_Canvas;
	}

	void NGraphRenderer::OnImGuiRender()
	{
		ST_CORE_ASSERT_MSG(!s_Context.m_DrawList, "Drawing list was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_Canvas, "Drawing canvas was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_PanelContext, "Context was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_PanelContext->GetController(), "Graph Controller not set!");

		NGraph* activeGraph = s_Context.m_PanelContext->GetActiveGraph();
		if (activeGraph)
		{
			for (auto& node : activeGraph->m_Nodes)
			{
				DrawNode(node);
			}
		}
	}

	void NGraphRenderer::SetDebugMode(bool debug)
	{
		s_Context.m_DebugOn = debug;
	}

	bool NGraphRenderer::IsDebugModeOn()
	{
		return s_Context.m_DebugOn;
	}

	void NGraphRenderer::DrawNode(std::shared_ptr<NNode> node)
	{
		ST_CORE_ASSERT_MSG(!s_Context.m_DrawList, "Drawing list was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_Canvas, "Drawing canvas was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_PanelContext, "Context was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_PanelContext->GetController(), "Graph Controller not set!");

		ImVec2 pos = node->GetPos();
		ImVec2 size = node->GetSize();
		bool hasHeader = node->HasHeader();

		// Draw the header
		if (hasHeader)
		{
			s_Context.m_DrawList->AddRectFilled(
				{ s_Context.m_Canvas->m_Origin.x + pos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pos.y * s_Context.m_Canvas->m_Scale },
				{ s_Context.m_Canvas->m_Origin.x + (pos.x + size.x) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (pos.y + size.y) * s_Context.m_Canvas->m_Scale },
				IM_COL32(20, 23, 20, 255),
				s_PanelStyle.m_NodeRounding * s_Context.m_Canvas->m_Scale);

			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			auto texture = s_Context.m_HeaderImage.get();
			if (texture)
			{
				s_Context.m_DrawList->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
					{ s_Context.m_Canvas->m_Origin.x + pos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pos.y * s_Context.m_Canvas->m_Scale },
					{ s_Context.m_Canvas->m_Origin.x + (pos.x + size.x) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (pos.y + s_PanelStyle.m_HeaderHeight) * s_Context.m_Canvas->m_Scale },
					uv_min, uv_max,
					IM_COL32(225, 30, 30, 255),
					s_PanelStyle.m_NodeRounding * s_Context.m_Canvas->m_Scale, ImDrawFlags_RoundCornersTop);
			}

			ImGui::SetWindowFontScale(s_Context.m_Canvas->m_Scale);
			s_Context.m_DrawList->AddText(
				{ s_Context.m_Canvas->m_Origin.x + (s_PanelStyle.m_HeaderTextOffset.x + pos.x) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (s_PanelStyle.m_HeaderTextOffset.y + pos.y) * s_Context.m_Canvas->m_Scale },
				IM_COL32(255, 255, 255, 255),
				node->GetTitle().c_str());
			ImGui::SetWindowFontScale(1.0f);
		}
		else
		{
			s_Context.m_DrawList->AddRectFilled(
				{ s_Context.m_Canvas->m_Origin.x + pos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pos.y * s_Context.m_Canvas->m_Scale },
				{ s_Context.m_Canvas->m_Origin.x + (pos.x + size.x) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (pos.y + size.y) * s_Context.m_Canvas->m_Scale },
				IM_COL32(20, 23, 20, 255),
				s_PanelStyle.m_NodeRounding * s_Context.m_Canvas->m_Scale);

			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			auto texture = s_Context.m_HeaderImage.get();
			if (texture)
			{
				ImU32 bgColor = IM_COL32(30, 225, 30, 255);
				/*if (!node->m_InPins.empty() || !node->m_OutPins.empty())
				{
					// Out pin has priority for color decision
					if (!node->m_OutPins.empty())
						bgColor = GetPinVariantColor(node->m_OutPins.front().get());
					else
						bgColor = GetPinVariantColor(node->m_InPins.front().get());
				}*/

				s_Context.m_DrawList->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
					{ s_Context.m_Canvas->m_Origin.x + pos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pos.y * s_Context.m_Canvas->m_Scale },
					{ s_Context.m_Canvas->m_Origin.x + (pos.x + size.x) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (pos.y + size.y) * s_Context.m_Canvas->m_Scale },
					uv_min, uv_max,
					bgColor,
					s_PanelStyle.m_NodeRounding * s_Context.m_Canvas->m_Scale);
			}
		}

		// Draw Pins and other components here

		// Draw frame if hovered over Node
		if (s_Context.m_PanelContext->IsNodeSelected(node))
		{
			s_Context.m_DrawList->AddRect(
				{ s_Context.m_Canvas->m_Origin.x + pos.x * s_Context.m_Canvas->m_Scale - s_PanelStyle.m_SelectionThinckness, s_Context.m_Canvas->m_Origin.y + pos.y * s_Context.m_Canvas->m_Scale - s_PanelStyle.m_SelectionThinckness },
				{ s_Context.m_Canvas->m_Origin.x + (pos.x + size.x) * s_Context.m_Canvas->m_Scale + s_PanelStyle.m_SelectionThinckness, s_Context.m_Canvas->m_Origin.y + (pos.y + size.y) * s_Context.m_Canvas->m_Scale + s_PanelStyle.m_SelectionThinckness },
				s_PanelStyle.m_NodeHoverColor,
				s_PanelStyle.m_NodeRounding * s_Context.m_Canvas->m_Scale, ImDrawFlags_RoundCornersAll, s_PanelStyle.m_SelectionThinckness);
		}
	}

}