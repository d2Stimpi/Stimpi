#include "stpch.h"
#include "Gui/NNode/NGraphStyle.h"

namespace Stimpi
{
	NNodePanelStyle s_PanelStyle;

	NNodePanelStyle::NNodePanelStyle()
	{
		m_HeaderHeight = 30.0f;
		m_HeaderTextOffset = { 35.0f, 8.0f };
		m_NodeHoverColor = IM_COL32(220, 220, 30, 255);

		m_NodeRounding = 15.0f;
		m_SelectionThinckness = 3.0f;

		float pinScale = 0.7f;
		m_PinOffset = 10.0f * pinScale; // Offset from Node frame side
		m_PinRadius = 8.0f * pinScale;
		m_PinThickness = 3.0f * pinScale;
		m_PinArrowSpacing = 2.0f * pinScale;
		m_PinArrowHalfHeight = 4.0f * pinScale;
		m_PinArrowWidth = 4.0f * pinScale;
		m_PinSpacing = 14.0f;
		m_PinTextSpacing = 4.0f;
		m_PinSelectOffset = 4.0f;

		m_ConnectionSegments = 20;

		m_LineThickness = 3.0f;

		m_GridStep = 16.0f;
	}
}