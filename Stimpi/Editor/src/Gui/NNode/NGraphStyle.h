#pragma once

#include "ImGui/src/imgui.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	struct NNodePanelStyle
	{
		// Node header
		float m_HeaderHeight;
		ImVec2 m_HeaderTextOffset;
		ImU32 m_NodeHoverColor;

		// Node frame
		float m_NodeRounding;
		float m_SelectionThinckness;

		// Pin
		float m_PinOffset;
		float m_PinRadius;
		float m_PinThickness;
		float m_PinArrowSpacing;
		float m_PinArrowHalfHeight;
		float m_PinArrowWidth;
		float m_PinSpacing;
		float m_PinTextSpacing; // For OUT pins
		float m_PinSelectOffset; // For higher Pin select precision

		// Connection
		uint32_t m_ConnectionSegments;

		// Lines
		float m_LineThickness;

		// Grid
		float m_GridStep;

		NNodePanelStyle();
	};

	extern NNodePanelStyle s_PanelStyle;
}