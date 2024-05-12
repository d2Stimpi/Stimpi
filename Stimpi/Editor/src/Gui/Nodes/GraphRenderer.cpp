#include "stpch.h"
#include "Gui/Nodes/GraphRenderer.h"

#include "Gui/Nodes/GraphPanel.h"

#include "Stimpi/Log.h"

namespace Stimpi
{

	GraphRenderer::GraphRenderer(GraphPanel* panelContext)
	{
		m_PanelContext = panelContext;
		m_HeaderImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/Gradient2.png");
	}

	GraphRenderer::~GraphRenderer()
	{

	}

	void GraphRenderer::SetDrawList(ImDrawList* drawList)
	{
		m_DrawList = drawList;
	}

	void GraphRenderer::SetDrawCanvas(GraphPanelCanvas* canvas)
	{
		m_Canvas = canvas;
	}

	void GraphRenderer::DrawNode(Node* node)
	{
		ST_ASSERT(!m_DrawList, "Drawing list was not set!");
		ST_ASSERT(!m_Canvas, "Drawing canvas was not set!");
		ST_ASSERT(!m_PanelContext, "Context was not set!");
		ImVec2 nodePos = node->m_Pos;

		// TODO: move it elsewhere, so its not calculated each frame
		// Calculate size of all Node's pins for drawing Node Rect
		ImVec2 pinSpaceSize = { 0.0f /*TODO: widht*/, 40.0f };
		// Check if Node can hold Pin space
		if (!IsNodeSpaceSizeAvailable(node, pinSpaceSize))
		{
			// Can't fit, resize node
			ResizeNodeSpace(node, pinSpaceSize);
		}

		m_DrawList->AddRectFilled(
			{ m_Canvas->m_Origin.x + nodePos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + nodePos.y * m_Canvas->m_Scale },
			{ m_Canvas->m_Origin.x + (nodePos.x + node->m_Size.x) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (nodePos.y + node->m_Size.y) * m_Canvas->m_Scale },
			IM_COL32(20, 23, 20, 255),
			15.0f * m_Canvas->m_Scale);

		ImVec2 uv_min = ImVec2(0.0f, 1.0f);
		ImVec2 uv_max = ImVec2(1.0f, 0.0f);
		auto texture = AssetManager::GetAsset(m_HeaderImage).As<Texture>();
		if (texture->Loaded())
		{
			m_DrawList->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
				{ m_Canvas->m_Origin.x + nodePos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + nodePos.y * m_Canvas->m_Scale },
				{ m_Canvas->m_Origin.x + (nodePos.x + node->m_Size.x) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (nodePos.y + s_Style.m_HeaderHeight) * m_Canvas->m_Scale },
				uv_min, uv_max,
				IM_COL32(225, 30, 30, 255),
				s_Style.m_NodeRounding * m_Canvas->m_Scale, ImDrawFlags_RoundCornersTop);
		}

		ImGui::SetWindowFontScale(m_Canvas->m_Scale);
		m_DrawList->AddText(
			{ m_Canvas->m_Origin.x + (s_Style.m_HeaderTextOffset.x + nodePos.x) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (s_Style.m_HeaderTextOffset.y + nodePos.y) * m_Canvas->m_Scale },
			IM_COL32(255, 255, 255, 255),
			node->m_Title.c_str());
		ImGui::SetWindowFontScale(1.0f);

		// Draw Node's pins here
		DrawNodePins(node);

		// Draw frame if hovered over Node
		if (m_PanelContext->IsNodeSelected(node))
		{
			m_DrawList->AddRect(
				{ m_Canvas->m_Origin.x + nodePos.x * m_Canvas->m_Scale - s_Style.m_SelectionThinckness, m_Canvas->m_Origin.y + nodePos.y * m_Canvas->m_Scale - s_Style.m_SelectionThinckness },
				{ m_Canvas->m_Origin.x + (nodePos.x + node->m_Size.x) * m_Canvas->m_Scale + s_Style.m_SelectionThinckness, m_Canvas->m_Origin.y + (nodePos.y + node->m_Size.y) * m_Canvas->m_Scale + s_Style.m_SelectionThinckness },
				s_Style.m_NodeHoverColor,
				s_Style.m_NodeRounding * m_Canvas->m_Scale, ImDrawFlags_RoundCornersAll, s_Style.m_SelectionThinckness);
		}
	}

	void GraphRenderer::DrawNodePins(Node* node)
	{

	}

	void GraphRenderer::DrawPin(Pin* pin, ImVec2 pos)
	{

	}

	void GraphRenderer::DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col /*= IM_COL32(255, 255, 255, 255)*/)
	{

	}

	void GraphRenderer::DrawPinToPinConnection(Pin* src, Pin* dest, ImU32 col /*= IM_COL32(255, 255, 255, 255)*/)
	{

	}

	void GraphRenderer::DrawDbgPoint(ImVec2 point)
	{

	}

	void GraphRenderer::DbgDrawConnectionLinePoints(PinConnection* connection)
	{

	}

}