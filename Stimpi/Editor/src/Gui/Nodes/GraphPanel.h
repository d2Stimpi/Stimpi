#pragma once

#include "Gui/Nodes/GraphComponents.h"
#include "Gui/Nodes/GraphRenderer.h"
#include "Gui/Nodes/GraphController.h"

namespace Stimpi
{
	// Canvas information for drawing space of the GraphPanel
	struct GraphPanelCanvas
	{
		ImVec2 m_PosMin = ImVec2(0.0f, 0.0f);
		ImVec2 m_PosMax = ImVec2(0.0f, 0.0f);
		ImVec2 m_Size = ImVec2(0.0f, 0.0f);

		ImVec2 m_Origin = { 0.0f, 0.0f };	// Global panel view translate offset
		ImVec2 m_Scrolling = { 0.0f, 0.0f };
		float m_Scale = 1.0f;

		GraphPanelCanvas() = default;
	};

	class GraphPanel
	{
	public:
		GraphPanel();
		~GraphPanel();

		void SetDrawList(ImDrawList* drawList);
		void OnImGuiRender();

		void DrawNodesPanel();
		void DrawDetailsPanel();
		void DrawGraph();
		void DrawGraphOverlay();

		void DrawVariableValueInput(Variable* pin);

		// Graph methods
		void AddGraph(Graph* graph);
		void RemoveGraph(Graph* graph);
		Graph* GetActiveGraph();

		static void ShowWindow(bool show);
		static bool IsVisible();
		static Graph* GetGlobalActiveGraph();

		// Node methods
		void CreateNode(ImVec2 pos, std::string title);
		void CreateNodeByName(ImVec2 pos, std::string title);
		void CreateNode(ImVec2 pos, std::string title, NodeLayout layout);
		void RemoveNode(Node* node);
		void PrepareRemoveNode(Node* node);

		bool IsNodeSelected(Node* node);
		void UpdateNodeConnectionsPoints(Node* node);

		void AddNodePopup(bool show);

		// Pin methods
		bool IsMouseHoveringPin(Pin* pin);
		Pin* GetMouseHoveredPin(Node* node);

		ImVec2 GetNodePanelViewClickLocation();
		bool IsMouseHoveringConnection(PinConnection* connection);
		PinConnection* GetMouseHoveredConnection();

		bool IsMouseHoverNode(Node* node);
		uint32_t GetMouseHoverNode();
		Node* GetNodeByID(uint32_t id);

		float GetPanelZoom();
		void SetPanelZoom(float zoom);
		void SetZoomEnable(bool enable);

		GraphContorller* GetController() { return m_GraphController; }

		// Specific handling in Panel when node is deleted
		void OnNodeDeleted(Node* deleted);
		void OnNodeDeselect();

	private:
		void SetCanvasData();
		void DrawCanvasGrid();

		void AddItemPopupButton(ImVec2 cursorPos, std::string name, std::function<void()> popupContent);
		void ItemRightClickPopup();

	private:
		static bool m_Show;		// Easier use in menu if static - to toggle window visibility
		static bool m_ShowNodesPanel;
		static bool m_ShowDetailsPanel;
		static bool m_ShowPopup;

		GraphRenderer* m_GraphRenderer = nullptr;
		GraphContorller* m_GraphController = nullptr;
	};
}