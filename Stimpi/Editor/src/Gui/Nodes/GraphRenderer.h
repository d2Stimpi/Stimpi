#pragma once

#include "Gui/Nodes/GraphComponents.h"

#include "Stimpi/Scene/Assets/AssetManager.h"

namespace Stimpi
{
	struct GraphPanelCanvas;
	class GraphPanel;

	class GraphRenderer
	{
	public:
		GraphRenderer(GraphPanel* panelContext);
		~GraphRenderer();

		void SetDrawList(ImDrawList* drawList);
		void SetDrawCanvas(GraphPanelCanvas* canvas);

		// Node methods
		void DrawNode(Node* node);

		// Pin methods
		void DrawNodePins(Node* node);
		void DrawPin(Pin* pin, ImVec2 pos);
		void DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col = IM_COL32(255, 255, 255, 255));
		void DrawPinToPinConnection(Pin* src, Pin* dest, ImU32 col = IM_COL32(255, 255, 255, 255));

		// Debug methods
		void DrawDbgPoint(ImVec2 point);
		void DbgDrawConnectionLinePoints(PinConnection* connection);

	private:
		ImDrawList* m_DrawList = nullptr;
		GraphPanel* m_PanelContext = nullptr;
		GraphPanelCanvas* m_Canvas = nullptr;

		AssetHandle m_HeaderImage;
	};
}