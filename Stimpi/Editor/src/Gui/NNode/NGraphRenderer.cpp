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
				DrawNode(node.get());
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

	void NGraphRenderer::DrawNode(NNode* node)
	{
		ST_CORE_ASSERT_MSG(!s_Context.m_DrawList, "Drawing list was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_Canvas, "Drawing canvas was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_PanelContext, "Context was not set!");
		ST_CORE_ASSERT_MSG(!s_Context.m_PanelContext->GetController(), "Graph Controller not set!");

		ImVec2 pos = node->m_Pos;
		ImVec2 size = node->m_Size;

		// Draw the header
		if (node->m_HasHeader)
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
				node->m_Title.c_str());
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
				if (!node->m_InPins.empty() || !node->m_OutPins.empty())
				{
					// Out pin has priority for color decision
					if (!node->m_OutPins.empty())
						bgColor = GetPinVariantColor(node->m_OutPins.front().get());
					else
						bgColor = GetPinVariantColor(node->m_InPins.front().get());
				}

				s_Context.m_DrawList->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
					{ s_Context.m_Canvas->m_Origin.x + pos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pos.y * s_Context.m_Canvas->m_Scale },
					{ s_Context.m_Canvas->m_Origin.x + (pos.x + size.x) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (pos.y + size.y) * s_Context.m_Canvas->m_Scale },
					uv_min, uv_max,
					bgColor,
					s_PanelStyle.m_NodeRounding * s_Context.m_Canvas->m_Scale);
			}
		}

		// Draw Pins and other components here
		DrawNodePins(node);

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

	void NGraphRenderer::DrawNodePins(NNode* node)
	{
		ImVec2 pinStartPos = node->m_Pos;
		pinStartPos.x += s_PanelStyle.m_PinOffset + s_PanelStyle.m_PinRadius;
		pinStartPos.y += s_PanelStyle.m_PinOffset + s_PanelStyle.m_PinRadius;
		if (node->m_HasHeader)
			pinStartPos.y += s_PanelStyle.m_HeaderHeight;

		for (auto& pin : node->m_InPins)
		{
			DrawPin(pin.get(), pinStartPos);
			pinStartPos.y += s_PanelStyle.m_PinSpacing + s_PanelStyle.m_PinRadius; // + radius because we draw from pin center
		}

		pinStartPos = node->m_Pos;
		pinStartPos.y += s_PanelStyle.m_PinOffset + s_PanelStyle.m_PinRadius;
		if (node->m_HasHeader)
			pinStartPos.y += s_PanelStyle.m_HeaderHeight;

		for (auto& pin : node->m_OutPins)
		{
			DrawPin(pin.get(), pinStartPos);
			pinStartPos.y += s_PanelStyle.m_PinSpacing + s_PanelStyle.m_PinRadius; // + radius because we draw from pin center
		}
	}

	void NGraphRenderer::DrawPin(NPin* pin, ImVec2 pos)
	{
		ImVec2 pinStartPos = pos;

		// Adjust position for OUT pin
		if (pin->m_Type == NPin::Type::Out)
		{
			ImVec2 nodeSize = pin->m_ParentNode->m_Size;
			// move draw cursor to the end of node space
			// assume correct Y pos is given already
			pinStartPos.x += nodeSize.x - s_PanelStyle.m_PinOffset - pin->GetPinSpaceWidth() + s_PanelStyle.m_PinRadius;
		}

		// Pin hit box pos
		pin->m_Pos = pinStartPos;

		// Circle part
		float circleSegments = 10.0f * s_Context.m_Canvas->m_Scale;
		circleSegments = (circleSegments > 5) ? ((circleSegments < 20) ? circleSegments : 20) : 5;
		if (pin->m_Connected)
		{
			s_Context.m_DrawList->AddCircleFilled(
				{ s_Context.m_Canvas->m_Origin.x + pinStartPos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pinStartPos.y * s_Context.m_Canvas->m_Scale },
				s_PanelStyle.m_PinRadius * s_Context.m_Canvas->m_Scale,
				GetPinVariantColor(pin),
				circleSegments);
		}
		else
		{
			s_Context.m_DrawList->AddCircle(
				{ s_Context.m_Canvas->m_Origin.x + pinStartPos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pinStartPos.y * s_Context.m_Canvas->m_Scale },
				s_PanelStyle.m_PinRadius * s_Context.m_Canvas->m_Scale,
				GetPinVariantColor(pin),
				circleSegments,
				s_PanelStyle.m_PinThickness * s_Context.m_Canvas->m_Scale);
		}

		// Arrow part
		ImVec2 pinArrowP1 = pinStartPos; // first point of arrow triangle - top
		pinArrowP1.x += s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinArrowSpacing;
		pinArrowP1.y -= s_PanelStyle.m_PinArrowHalfHeight;
		ImVec2 pinArrowP2 = pinStartPos; // middle
		pinArrowP2.x += s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinArrowSpacing + s_PanelStyle.m_PinArrowWidth;
		ImVec2 pinArrowP3 = pinStartPos; // bottom
		pinArrowP3.x += s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinArrowSpacing;
		pinArrowP3.y += s_PanelStyle.m_PinArrowHalfHeight;

		s_Context.m_DrawList->AddTriangleFilled(
			{ s_Context.m_Canvas->m_Origin.x + pinArrowP1.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pinArrowP1.y * s_Context.m_Canvas->m_Scale },
			{ s_Context.m_Canvas->m_Origin.x + pinArrowP2.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pinArrowP2.y * s_Context.m_Canvas->m_Scale },
			{ s_Context.m_Canvas->m_Origin.x + pinArrowP3.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + pinArrowP3.y * s_Context.m_Canvas->m_Scale },
			GetPinVariantColor(pin));

		// Pin text part
		if (pin->m_Type == NPin::Type::Out || pin->m_Type == NPin::Type::In)
		{
			ImVec2 textPos = pinStartPos;
			ImVec2 textSize = ImGui::CalcTextSize(pin->m_Label.c_str());
			textSize.x += s_PanelStyle.m_PinTextSpacing;

			if (pin->m_Type == NPin::Type::Out)
			{
				textPos.x -= s_PanelStyle.m_PinRadius + textSize.x;
				textPos.y -= s_PanelStyle.m_PinRadius + 1.0f;
			}
			else
			{
				textPos.x += pin->GetPinSpaceWidth();
				textPos.y -= s_PanelStyle.m_PinRadius + 1.0f;
			}

			ImGui::SetWindowFontScale(s_Context.m_Canvas->m_Scale);
			s_Context.m_DrawList->AddText(
				{ s_Context.m_Canvas->m_Origin.x + textPos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + textPos.y * s_Context.m_Canvas->m_Scale },
				IM_COL32(255, 255, 255, 255),
				pin->m_Label.c_str());
			ImGui::SetWindowFontScale(1.0f);
		}

		// Draw Pin <-> FloatingTarget connection
		if (pin == s_Context.m_PanelContext->GetController()->GetSelectedPin() && s_Context.m_PanelContext->GetController()->GetAction() == NControllAction::NODE_PIN_DRAG)
		{
			ImVec2 startPoint = pin->m_Pos;
			ImVec2 endPoint = s_Context.m_PanelContext->GetController()->GetPinFloatingTarget();

			DrawBezierLine(startPoint, endPoint);
		}
		// Draw pin connections
		for (auto& target : pin->m_ConnectedPins)
		{
			if (target)
			{
				DrawPinToPinConnection(pin, target.get());
			}
		}

		// Draw debug layer
		if (s_Context.m_DebugOn)
		{
			s_Context.m_DrawList->AddRect(
				{ s_Context.m_Canvas->m_Origin.x + (pin->m_Pos.x - s_PanelStyle.m_PinRadius) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (pin->m_Pos.y - s_PanelStyle.m_PinRadius) * s_Context.m_Canvas->m_Scale },
				{ s_Context.m_Canvas->m_Origin.x + (pin->m_Pos.x + s_PanelStyle.m_PinRadius) * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + (pin->m_Pos.y + s_PanelStyle.m_PinRadius) * s_Context.m_Canvas->m_Scale },
				IM_COL32(225, 25, 25, 255));

			std::string dbgText = fmt::format("C: {}", pin->m_ConnectedPins.size());
			ImVec2 dbgTextPos = pinStartPos;
			dbgTextPos.x = pin->m_Type == NPin::Type::In ?
				dbgTextPos.x - pin->GetPinSpaceWidth() - ImGui::CalcTextSize(dbgText.c_str()).x :
				dbgTextPos.x + pin->GetPinSpaceWidth();

			ImGui::SetWindowFontScale(s_Context.m_Canvas->m_Scale);
			s_Context.m_DrawList->AddText(
				{ s_Context.m_Canvas->m_Origin.x + dbgTextPos.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + dbgTextPos.y * s_Context.m_Canvas->m_Scale },
				IM_COL32(255, 25, 25, 255),
				dbgText.c_str());
			ImGui::SetWindowFontScale(1.0f);
		}
	}

	void NGraphRenderer::DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col /*= IM_COL32(255, 255, 255, 255)*/)
	{
		ImVec2 startPoint = start;
		ImVec2 endPoint = end;
		ImVec2 middlePoint1 = CalcFirstMidBezierPoint(startPoint, endPoint);
		ImVec2 middlePoint2 = CalcLastMidBezierPoint(startPoint, endPoint);
		float lineThickness = s_PanelStyle.m_LineThickness * s_Context.m_Canvas->m_Scale;
		s_Context.m_DrawList->AddBezierCubic(
			{ s_Context.m_Canvas->m_Origin.x + startPoint.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + startPoint.y * s_Context.m_Canvas->m_Scale },
			{ s_Context.m_Canvas->m_Origin.x + middlePoint1.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + middlePoint1.y * s_Context.m_Canvas->m_Scale },
			{ s_Context.m_Canvas->m_Origin.x + middlePoint2.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + middlePoint2.y * s_Context.m_Canvas->m_Scale },
			{ s_Context.m_Canvas->m_Origin.x + endPoint.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + endPoint.y * s_Context.m_Canvas->m_Scale },
			col,
			lineThickness,
			s_PanelStyle.m_ConnectionSegments);

		if (s_Context.m_DebugOn)
		{
			s_Context.m_DrawList->AddCircleFilled(
				{ s_Context.m_Canvas->m_Origin.x + startPoint.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + startPoint.y * s_Context.m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
			s_Context.m_DrawList->AddCircleFilled(
				{ s_Context.m_Canvas->m_Origin.x + middlePoint1.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + middlePoint1.y * s_Context.m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
			s_Context.m_DrawList->AddCircleFilled(
				{ s_Context.m_Canvas->m_Origin.x + middlePoint2.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + middlePoint2.y * s_Context.m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
			s_Context.m_DrawList->AddCircleFilled(
				{ s_Context.m_Canvas->m_Origin.x + endPoint.x * s_Context.m_Canvas->m_Scale, s_Context.m_Canvas->m_Origin.y + endPoint.y * s_Context.m_Canvas->m_Scale },
				lineThickness, IM_COL32(225, 25, 25, 255), 6);
		}
	}

	void NGraphRenderer::DrawPinToPinConnection(NPin* src, NPin* dest, ImU32 col /*= IM_COL32(255, 255, 255, 255)*/)
	{
		if (src == nullptr || dest == nullptr)
			return;

		ImVec2 startPoint = src->m_Pos;
		ImVec2 endPoint = dest->m_Pos;

		DrawBezierLine(startPoint, endPoint, col);
	}

	ImU32 NGraphRenderer::GetPinVariantColor(NPin* pin)
	{
		ImU32 picked = IM_COL32(35, 195, 35, 255);

		/*switch (pin->m_Variable->m_ValueType)
		{
		case Variable::ValueType::None: // No custom color
		case Variable::ValueType::Flow: // No custom color
			break;
		case Variable::ValueType::Bool:		picked = IM_COL32(35, 195, 35, 255);	break;
		case Variable::ValueType::Int:		picked = IM_COL32(35, 35, 195, 255);	break;
		case Variable::ValueType::Float:	picked = IM_COL32(195, 35, 35, 255);	break;
		case Variable::ValueType::Vector2:	picked = IM_COL32(195, 195, 35, 255);	break;
		case Variable::ValueType::String:	picked = IM_COL32(35, 195, 195, 255);	break;
		}*/

		return picked;
	}

	ImVec2 NGraphRenderer::CalcFirstMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, start.y };
	}

	ImVec2 NGraphRenderer::CalcLastMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, end.y };
	}

}