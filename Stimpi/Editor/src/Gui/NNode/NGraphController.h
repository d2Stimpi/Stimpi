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

		std::shared_ptr<NNode> GetSelectedNode();
		NPin* GetSelectedPin();
		ImVec2& GetPinFloatingTarget();

		NControllAction GetAction();

	private:
		bool m_IsActive = false;
		NGraphPanel* m_PanelContext = nullptr;
		NGraphPanelCanvas* m_Canvas = nullptr;

		NControllAction m_Action = NControllAction::NONE;

		NGraph* m_Graph = nullptr;
		std::shared_ptr<NNode> m_SelectedNode = nullptr;
		std::shared_ptr<NPin> m_SelectedPin = nullptr;
		std::shared_ptr<NPinConnection> m_SelectedConnection = nullptr;
		ImVec2 m_PinFloatingTarget = { 0.0f, 0.0f };
	};
}