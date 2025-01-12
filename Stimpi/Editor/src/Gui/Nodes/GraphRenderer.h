#pragma once

#include "Gui/Nodes/GraphComponents.h"
#include "Stimpi/Graphics/Texture.h"

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

		void OnImGuiRender();

		// Node methods
		void DrawNode(Node* node);

		// Pin methods
		void DrawNodePins(Node* node);
		void DrawPin(Pin* pin, ImVec2 pos);
		void DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col = IM_COL32(255, 255, 255, 255));
		void DrawPinToPinConnection(Pin* src, Pin* dest, ImU32 col = IM_COL32(255, 255, 255, 255));

		// Debug methods
		void SetDebugMode(bool debug) { m_DebugOn = debug; }
		bool IsDebugModeOn() { return m_DebugOn; }
		void DrawDbgPoint(ImVec2 point);
		void DbgDrawConnectionLinePoints(PinConnection* connection);

	private:
		ImDrawList* m_DrawList = nullptr;
		GraphPanel* m_PanelContext = nullptr;
		GraphPanelCanvas* m_Canvas = nullptr;

		std::shared_ptr<Texture> m_HeaderImage;
		std::shared_ptr<Texture> m_HighlightImage;

		bool m_DebugOn = false;
	};
}