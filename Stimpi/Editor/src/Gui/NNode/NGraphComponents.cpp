#include "stpch.h"
#include "Gui/NNode/NGraphComponents.h"

#include "Gui/NNode/NGraphRenderer.h"
#include "Gui/NNode/NGraphStyle.h"

namespace Stimpi
{

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

	void NNode::AddPin(NPin::Type type, const std::string& title, const NPin::DataType& dataType)
	{
		if (type == NPin::Type::In)
		{
			m_InPins.push_back(std::make_shared<NPin>(this, NPin::Type::In, title, dataType));
		}
		else if (type == NPin::Type::Out)
		{
			m_OutPins.push_back(std::make_shared<NPin>(this, NPin::Type::Out, title, dataType));
		}
	}


	bool NNode::HasConnection()
	{
		return HasInputConnection() || HasOutputConnection();
	}

	bool NNode::HasInputConnection()
	{
		for (auto& in : m_InPins)
		{
			if (in->m_Connected)
				return true;
		}

		return false;
	}

	bool NNode::HasOutputConnection()
	{
		for (auto& out : m_OutPins)
		{
			if (out->m_Connected)
				return true;
		}

		return false;
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


	/**
	 * PinConnection methods
	 */
	
	
	ImVec2 NPinConnection::CalcFirstMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, start.y };
	}

	ImVec2 NPinConnection::CalcLastMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, end.y };
	}

	ImVec2 NPinConnection::BezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
	{
		float u = 1.0f - t;
		float w1 = u * u * u;
		float w2 = 3 * u * u * t;
		float w3 = 3 * u * t * t;
		float w4 = t * t * t;
		return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y);
	}

	void NPinConnection::CalculateBezierPoints(uint32_t segments)
	{
		ImVec2 startPoint = m_Src->m_Pos;
		ImVec2 endPoint = m_Dest->m_Pos;
		ImVec2 middlePoint1 = CalcFirstMidBezierPoint(startPoint, endPoint);
		ImVec2 middlePoint2 = CalcLastMidBezierPoint(startPoint, endPoint);

		m_BezierLinePoints.push_back(startPoint);

		float t_step = 1.0f / (float)segments;
		for (int i_step = 1; i_step <= segments; i_step++)
			m_BezierLinePoints.push_back(BezierCubicCalc(startPoint, middlePoint1, middlePoint2, endPoint, t_step * i_step));
	}

	void NPinConnection::UpdateConnectionPoints()
	{
		m_BezierLinePoints.clear();
		CalculateBezierPoints(s_PanelStyle.m_ConnectionSegments);
	}

}