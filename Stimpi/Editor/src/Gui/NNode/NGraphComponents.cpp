#include "stpch.h"
#include "Gui/NNode/NGraphComponents.h"

#include "Gui/NNode/NGraphRenderer.h"
#include "Gui/NNode/NGraphStyle.h"

namespace Stimpi
{

	NNode::~NNode()
	{

	}

	ImVec2 NNode::CalcNodeSize()
	{
		ImVec2 size = { 0.0f, 0.0f };

		// Pin space
		int pinCount = std::max(m_InPins.size(), m_OutPins.size());
		size.y += pinCount * (s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinSpacing) + s_PanelStyle.m_PinOffset;

		// Width
		if (!m_InPins.empty())
		{
			// Find longest text pin
			float maxWidth = 0.0f;
			for (auto& pin : m_InPins)
			{
				ImVec2 textSize = ImGui::CalcTextSize(pin->m_Label.c_str());
				maxWidth = std::max(textSize.x, maxWidth);
			}

			size.x += s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinArrowSpacing + s_PanelStyle.m_PinArrowWidth + s_PanelStyle.m_PinOffset + s_PanelStyle.m_PinTextSpacing;
			size.x += maxWidth;
		}

		if (!m_OutPins.empty())
		{
			// Find longest text pin
			float maxWidth = 0.0f;
			for (auto& pin : m_OutPins)
			{
				ImVec2 textSize = ImGui::CalcTextSize(pin->m_Label.c_str());
				maxWidth = std::max(textSize.x, maxWidth);
			}

			size.x += s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinArrowSpacing + s_PanelStyle.m_PinArrowWidth + s_PanelStyle.m_PinOffset + s_PanelStyle.m_PinTextSpacing;
			size.x += maxWidth;
		}

		if (!m_OutPins.empty() && !m_InPins.empty())
		{
			size.x += s_PanelStyle.m_PinSpacing * 2;
		}

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

		m_Size = size;
		return size;
	}


	/**
	 * Pin methods
	 */


	float NPin::GetPinSpaceHeight()
	{
		return s_PanelStyle.m_PinRadius * 2.0f;
	}

	float NPin::GetPinSpaceWidth()
	{
		return s_PanelStyle.m_PinRadius * 2.0f + s_PanelStyle.m_PinArrowSpacing + s_PanelStyle.m_PinArrowWidth;
	}

}