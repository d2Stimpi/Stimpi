#include "stpch.h"
#include "Gui/NNode/NGraphComponents.h"

#include "Gui/NNode/NGraphRenderer.h"
#include "Gui/NNode/NGraphStyle.h"

namespace Stimpi
{

	NNode::~NNode()
	{

	}

	void NNode::OnImGuiRender()
	{
		NGraphRenderer::BeginNode(m_Title, m_HasHeader, m_Pos, m_Size);

		NGraphRenderer::EndNode();
	}

	ImVec2 NNode::CalcNodeSize()
	{
		ImVec2 size = { 0.0f, 0.0f };

		// Pin space
		int pinCount = 0; //std::max(node->m_InPins.size(), node->m_OutPins.size());
		size.y += pinCount * (s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinSpacing) + s_PanelStyle.m_PinOffset;

		// Width
		/*if (!node->m_InPins.empty())
		{
			// Find longest text pin
			float maxWidth = 0.0f;
			for (auto& pin : node->m_InPins)
			{
				ImVec2 textSize = ImGui::CalcTextSize(pin->m_Variable->m_Name.c_str());
				maxWidth = std::max(textSize.x, maxWidth);
			}

			size.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth + s_Style.m_PinOffset + s_Style.m_PinTextSpacing;
			size.x += maxWidth;
		}

		if (!node->m_OutPins.empty())
		{
			// Find longest text pin
			float maxWidth = 0.0f;
			for (auto& pin : node->m_OutPins)
			{
				ImVec2 textSize = ImGui::CalcTextSize(pin->m_Variable->m_Name.c_str());
				maxWidth = std::max(textSize.x, maxWidth);
			}

			size.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth + s_Style.m_PinOffset + s_Style.m_PinTextSpacing;
			size.x += maxWidth;
		}

		if (!node->m_OutPins.empty() && !node->m_InPins.empty())
		{
			size.x += s_Style.m_PinSpacing * 2;
		}*/

		if (m_HasHeader)
		{
			size.y += s_PanelStyle.m_HeaderHeight + 10;

			float width = 2 * s_PanelStyle.m_HeaderTextOffset.x;
			width += ImGui::CalcTextSize(m_Title.c_str()).x;

			if (width > size.x)
			{
				size.x = width;
			}
		}
		else
		{
			size.x += s_PanelStyle.m_HeaderTextOffset.x;
		}

		return size;
	}

}