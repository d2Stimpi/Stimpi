#include "stpch.h"
#include "Gui/Nodes/GraphRenderer.h"

#include "Gui/Nodes/GraphPanel.h"

#include "Stimpi/Log.h"

namespace Stimpi
{
	static ImU32 GetPinVariantColor(Pin* pin)
	{
		ImU32 picked = IM_COL32(35, 195, 35, 255);
		pin_type_variant val = pin->m_Value;

		if (std::holds_alternative<bool>(val))
		{
			picked = IM_COL32(35, 195, 35, 255);
		}

		if (std::holds_alternative<int>(val))
		{
			picked = IM_COL32(35, 35, 195, 255);
		}

		if (std::holds_alternative<float>(val))
		{
			picked = IM_COL32(195, 35, 35, 255);
		}

		if (std::holds_alternative<glm::vec2>(val))
		{
			picked = IM_COL32(195, 195, 35, 255);
		}

		if (std::holds_alternative<std::string>(val))
		{
			picked = IM_COL32(35, 195, 195, 255);
		}

		return picked;
	}

	GraphRenderer::GraphRenderer(GraphPanel* panelContext)
	{
		m_PanelContext = panelContext;
		m_HeaderImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/Gradient2.png");
		m_HighlightImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/Highlight.png");
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

	void GraphRenderer::OnImGuiRender()
	{
		Graph* activeGraph = m_PanelContext->GetActiveGraph();
		if (activeGraph)
		{
			for (auto& node : activeGraph->m_Nodes)
			{
				DrawNode(node.get());
			}
		}
	}

	void GraphRenderer::DrawNode(Node* node)
	{
		ST_ASSERT(!m_DrawList, "Drawing list was not set!");
		ST_ASSERT(!m_Canvas, "Drawing canvas was not set!");
		ST_ASSERT(!m_PanelContext, "Context was not set!");
		ST_ASSERT(!m_PanelContext->GetController(), "Graph Controller not set!");
		ImVec2 nodePos = node->m_Pos;


		if (node->m_HasHeader)
		{
			m_DrawList->AddRectFilled(
				{ m_Canvas->m_Origin.x + nodePos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + nodePos.y * m_Canvas->m_Scale },
				{ m_Canvas->m_Origin.x + (nodePos.x + node->m_Size.x) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (nodePos.y + node->m_Size.y) * m_Canvas->m_Scale },
				IM_COL32(20, 23, 20, 255),
				s_Style.m_NodeRounding* m_Canvas->m_Scale);

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
		}
		else
		{
			m_DrawList->AddRectFilled(
				{ m_Canvas->m_Origin.x + nodePos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + nodePos.y * m_Canvas->m_Scale },
				{ m_Canvas->m_Origin.x + (nodePos.x + node->m_Size.x) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (nodePos.y + node->m_Size.y) * m_Canvas->m_Scale },
				IM_COL32(20, 23, 20, 255),
				s_Style.m_NodeRounding * m_Canvas->m_Scale);

			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			auto texture = AssetManager::GetAsset(m_HighlightImage).As<Texture>();
			if (texture->Loaded())
			{
				ImU32 bgColor = IM_COL32(30, 225, 30, 255);
				if (!node->m_InPins.empty() || !node->m_OutPins.empty())
				{
					// Out pin has priority for color decision
					if (!node->m_OutPins.empty())
						bgColor = GetPinVariantColor(node->m_OutPins.front().get());
					else
						bgColor = GetPinVariantColor(node->m_InPins.front().get());
				}

				m_DrawList->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
					{ m_Canvas->m_Origin.x + nodePos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + nodePos.y * m_Canvas->m_Scale },
					{ m_Canvas->m_Origin.x + (nodePos.x + node->m_Size.x) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (nodePos.y + node->m_Size.y) * m_Canvas->m_Scale },
					uv_min, uv_max, 
					bgColor,
					s_Style.m_NodeRounding* m_Canvas->m_Scale);
			}
		}

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
		ImVec2 pinStartPos = node->m_Pos;
		pinStartPos.x += s_Style.m_PinOffset + s_Style.m_PinRadius;
		pinStartPos.y += s_Style.m_PinOffset + s_Style.m_PinRadius;
		if (node->m_HasHeader)
			pinStartPos.y += s_Style.m_HeaderHeight;

		for (auto pin : node->m_InPins)
		{
			DrawPin(pin.get(), pinStartPos);
			pinStartPos.y += s_Style.m_PinSpacing + s_Style.m_PinRadius; // + radius because we draw from pin center
		}

		pinStartPos = node->m_Pos;
		pinStartPos.y += s_Style.m_PinOffset + s_Style.m_PinRadius;
		if (node->m_HasHeader)
			pinStartPos.y += s_Style.m_HeaderHeight;

		for (auto pin : node->m_OutPins)
		{
			DrawPin(pin.get(), pinStartPos);
			pinStartPos.y += s_Style.m_PinSpacing + s_Style.m_PinRadius; // + radius because we draw from pin center
		}
	}

	void GraphRenderer::DrawPin(Pin* pin, ImVec2 pos)
	{
		ImVec2 pinStartPos = pos;

		// Adjust position for OUT pin
		if (pin->m_Type == Pin::Type::OUTPUT)
		{
			ImVec2 nodeSize = pin->m_ParentNode->m_Size;
			// move draw cursor to the end of node space
			// assume correct Y pos is given already
			pinStartPos.x += nodeSize.x - s_Style.m_PinOffset - GetPinSpaceWidth() + s_Style.m_PinRadius;
		}

		// Pin hit box pos
		pin->m_Pos = pinStartPos;

		// Circle part
		float circleSegments = 10.0f * m_Canvas->m_Scale;
		circleSegments = (circleSegments > 5) ? ((circleSegments < 20) ? circleSegments : 20) : 5;
		if (pin->m_Connected)
		{
			m_DrawList->AddCircleFilled(
				{ m_Canvas->m_Origin.x + pinStartPos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + pinStartPos.y * m_Canvas->m_Scale },
				s_Style.m_PinRadius * m_Canvas->m_Scale,
				GetPinVariantColor(pin),
				circleSegments);
		}
		else
		{
			m_DrawList->AddCircle(
				{ m_Canvas->m_Origin.x + pinStartPos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + pinStartPos.y * m_Canvas->m_Scale },
				s_Style.m_PinRadius * m_Canvas->m_Scale,
				GetPinVariantColor(pin),
				circleSegments,
				s_Style.m_PinThickness * m_Canvas->m_Scale);
		}

		// Arrow part
		ImVec2 pinArrowP1 = pinStartPos; // first point of arrow triangle - top
		pinArrowP1.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing;
		pinArrowP1.y -= s_Style.m_PinArrowHalfHeight;
		ImVec2 pinArrowP2 = pinStartPos; // middle
		pinArrowP2.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth;
		ImVec2 pinArrowP3 = pinStartPos; // bottom
		pinArrowP3.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing;
		pinArrowP3.y += s_Style.m_PinArrowHalfHeight;

		m_DrawList->AddTriangleFilled(
			{ m_Canvas->m_Origin.x + pinArrowP1.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + pinArrowP1.y * m_Canvas->m_Scale },
			{ m_Canvas->m_Origin.x + pinArrowP2.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + pinArrowP2.y * m_Canvas->m_Scale },
			{ m_Canvas->m_Origin.x + pinArrowP3.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + pinArrowP3.y * m_Canvas->m_Scale },
			GetPinVariantColor(pin));

		// Pin text part
		ImVec2 textPos = pinStartPos;
		ImVec2 textSize = ImGui::CalcTextSize(pin->m_Text.c_str());
		textSize.x += s_Style.m_PinTextSpacing;

		if (pin->m_Type == Pin::Type::OUTPUT)
		{
			textPos.x -= s_Style.m_PinRadius + textSize.x;
			textPos.y -= s_Style.m_PinRadius + 1.0f;
		}
		else
		{
			textPos.x += GetPinSpaceWidth();
			textPos.y -= s_Style.m_PinRadius + 1.0f; // TODO: consider changing
		}

		ImGui::SetWindowFontScale(m_Canvas->m_Scale);
		m_DrawList->AddText(
			{ m_Canvas->m_Origin.x + textPos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + textPos.y * m_Canvas->m_Scale },
			IM_COL32(255, 255, 255, 255),
			pin->m_Text.c_str());
		ImGui::SetWindowFontScale(1.0f);

		// Test draw Pin <-> FloatingTarget connection
		if (pin == m_PanelContext->GetController()->GetSelectedPin() && m_PanelContext->GetController()->GetAction() == ControllAction::NODE_PIN_DRAG)
		{
			ImVec2 startPoint = pin->m_Pos;
			ImVec2 endPoint = m_PanelContext->GetController()->GetPinFloatingTarget();

			DrawBezierLine(startPoint, endPoint);
		}

		// Draw pin connections
		for (auto target : pin->m_ConnectedPins)
		{
			if (target)
			{
				DrawPinToPinConnection(pin, target);
			}
		}

		// Draw debug layer
		if (m_DebugOn)
		{
			m_DrawList->AddRect(
				{ m_Canvas->m_Origin.x + (pin->m_Pos.x - s_Style.m_PinRadius) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (pin->m_Pos.y - s_Style.m_PinRadius) * m_Canvas->m_Scale },
				{ m_Canvas->m_Origin.x + (pin->m_Pos.x + s_Style.m_PinRadius) * m_Canvas->m_Scale, m_Canvas->m_Origin.y + (pin->m_Pos.y + s_Style.m_PinRadius) * m_Canvas->m_Scale },
				IM_COL32(225, 25, 25, 255));

			std::string dbgText = fmt::format("C: {}", pin->m_ConnectedPins.size());
			ImVec2 dbgTextPos = pinStartPos;
			dbgTextPos.x = pin->m_Type == Pin::Type::INPUT ?
				dbgTextPos.x - GetPinSpaceWidth() - ImGui::CalcTextSize(dbgText.c_str()).x :
				dbgTextPos.x + GetPinSpaceWidth();

			ImGui::SetWindowFontScale(m_Canvas->m_Scale);
			m_DrawList->AddText(
				{ m_Canvas->m_Origin.x + dbgTextPos.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + dbgTextPos.y * m_Canvas->m_Scale },
				IM_COL32(255, 25, 25, 255),
				dbgText.c_str());
			ImGui::SetWindowFontScale(1.0f);
		}
	}

	void GraphRenderer::DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col /*= IM_COL32(255, 255, 255, 255)*/)
	{
		ImVec2 startPoint = start;
		ImVec2 endPoint = end;
		ImVec2 middlePoint1 = CalcFirstMidBezierPoint(startPoint, endPoint);
		ImVec2 middlePoint2 = CalcLastMidBezierPoint(startPoint, endPoint);
		float lineThickness = s_Style.m_LineThickness * m_Canvas->m_Scale;
		m_DrawList->AddBezierCubic(
			{ m_Canvas->m_Origin.x + startPoint.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + startPoint.y * m_Canvas->m_Scale },
			{ m_Canvas->m_Origin.x + middlePoint1.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + middlePoint1.y * m_Canvas->m_Scale },
			{ m_Canvas->m_Origin.x + middlePoint2.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + middlePoint2.y * m_Canvas->m_Scale },
			{ m_Canvas->m_Origin.x + endPoint.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + endPoint.y * m_Canvas->m_Scale },
			col,
			lineThickness,
			s_Style.m_ConnectionSegments);

		if (m_DebugOn)
		{
			m_DrawList->AddCircleFilled(
				{ m_Canvas->m_Origin.x + startPoint.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + startPoint.y * m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
			m_DrawList->AddCircleFilled(
				{ m_Canvas->m_Origin.x + middlePoint1.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + middlePoint1.y * m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
			m_DrawList->AddCircleFilled(
				{ m_Canvas->m_Origin.x + middlePoint2.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + middlePoint2.y * m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
			m_DrawList->AddCircleFilled(
				{ m_Canvas->m_Origin.x + endPoint.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + endPoint.y * m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
		}
	}

	void GraphRenderer::DrawPinToPinConnection(Pin* src, Pin* dest, ImU32 col /*= IM_COL32(255, 255, 255, 255)*/)
	{
		if (src == nullptr || dest == nullptr)
			return;

		ImVec2 startPoint = src->m_Pos;
		ImVec2 endPoint = dest->m_Pos;

		DrawBezierLine(startPoint, endPoint, col);
	}

	void GraphRenderer::DrawDbgPoint(ImVec2 point)
	{
		m_DrawList->AddCircleFilled(
			{ m_Canvas->m_Origin.x + point.x * m_Canvas->m_Scale, m_Canvas->m_Origin.y + point.y * m_Canvas->m_Scale },
			s_Style.m_LineThickness* m_Canvas->m_Scale, IM_COL32(225, 25, 25, 255), 6);
	}

	void GraphRenderer::DbgDrawConnectionLinePoints(PinConnection* connection)
	{
		if (connection && m_DebugOn)
		{
			for (auto point : connection->m_BezierLinePoints)
			{
				DrawDbgPoint(point);
			}
		}
	}

}