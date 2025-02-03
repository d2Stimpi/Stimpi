#pragma once

#include "Gui/NNode/NGraph.h"

namespace Stimpi
{
	// Forwards to avoid include issues
	struct NGraphPanelCanvas;
	class NGraphPanel;

	enum class NControllAction { NONE = 0, NODE_HOVER, NODE_DRAGABLE, SHOW_POPUP_ONRELEASE, POPUP_ACTIVE, CANVAS_MOVE, NODE_PIN_DRAG, CONNECTION_ONPRESS };

	class NGraphController
	{
	public:
		NGraphController(NGraphPanel* panelContext);
		~NGraphController();

		void SetDrawCanvas(NGraphPanelCanvas* canvas);
		void SetActiveGraph(NGraph* graph);

		void UpdateMouseControls();
		void HandleKeyPresses();

		void SetActive(bool active) { m_IsActive = active; }

		NControllAction GetAction();

	private:
		bool m_IsActive = false;
		NGraphPanel* m_PanelContext = nullptr;
		NGraphPanelCanvas* m_Canvas = nullptr;

		NControllAction m_Action = NControllAction::NONE;

		NGraph* m_Graph = nullptr;
		NNode* m_SelectedNode = nullptr;
	};
}