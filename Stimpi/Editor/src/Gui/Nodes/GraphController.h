#pragma once

#include "Gui/Nodes/GraphComponents.h"

namespace Stimpi
{
	struct GraphPanelCanvas;
	class GraphPanel;

	enum class ControllAction { NONE = 0, NODE_HOVER, NODE_DRAGABLE, SHOW_POPUP_ONRELEASE, CANVAS_MOVE, NODE_PIN_DRAG, CONNECTION_ONPRESS };

	class GraphContorller
	{
	public:
		GraphContorller(GraphPanel* panelContext);
		~GraphContorller();

		void SetDrawCanvas(GraphPanelCanvas* canvas);
		void SetActiveGraph(Graph* graph);

		void UpdateMouseControls();
		void HandleKeyPresses();

		void SetActive(bool active) { m_IsActive = active; }

		Node* GetSelectedNode();
		Pin* GetSelectedPin();

		ControllAction GetAction();
		PinConnection* GetSeletedPinConnection();
		ImVec2& GetPinFloatingTarget();

	private:
		void UpdateInternals();

	private:
		bool m_IsActive = false;
		GraphPanel* m_PanelContext = nullptr;
		GraphPanelCanvas* m_Canvas = nullptr;

		ControllAction m_Action = ControllAction::NONE;

		Graph* m_Graph = nullptr;
		Node* m_SelectedNode = nullptr;
		Pin* m_SelectedPin = nullptr;
		PinConnection* m_SelectedConnection = nullptr;
		ImVec2 m_PinFloatingTarget = { 0.0f, 0.0f };
	};
}