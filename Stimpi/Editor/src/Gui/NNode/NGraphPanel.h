#pragma once

#include "Gui/NNode/NGraphController.h"
#include "Gui/NNode/NGraph.h"

namespace Stimpi
{
	// Canvas information for drawing space of the GraphPanel
	struct NGraphPanelCanvas
	{
		ImVec2 m_PosMin = ImVec2(0.0f, 0.0f);
		ImVec2 m_PosMax = ImVec2(0.0f, 0.0f);
		ImVec2 m_Size = ImVec2(0.0f, 0.0f);

		ImVec2 m_Origin = { 0.0f, 0.0f };	// Global panel view translate offset
		ImVec2 m_Scrolling = { 0.0f, 0.0f };
		float m_Scale = 1.0f;

		NGraphPanelCanvas() = default;
	};

	class NGraphPanel
	{
	public:
		NGraphPanel();
		~NGraphPanel();

		void SetDrawList(ImDrawList* drawList);
		void OnImGuiRender();

		void DrawNodesPanel();
		void DrawDetailsPanel();
		void DrawGraph();
		void DrawGraphOverlay();

		static void ShowWindow(bool show);
		static bool IsVisible();

		void AddNodePopup(bool show);
		ImVec2 GetNodePanelViewMouseLocation();

		// Graph methods
		void AddGraph(std::shared_ptr<NGraph> graph);
		NGraph* GetActiveGraph();

		NGraphController* GetController() { return m_GraphController; }

		// Controller helpers
		bool IsMouseHoverNode(NNode* node);
		NNodeId GetMouseHoverNode();

		bool IsMouseHoveringConnection(std::shared_ptr<NPinConnection> connection);
		std::shared_ptr<NPinConnection> GetMouseHoveredConnection();

		std::shared_ptr<NNode> GetNodeByID(NNodeId id);

		float GetPanelZoom();
		void SetPanelZoom(float zoom);
		void SetZoomEnable(bool enable);

		// Node methods
		void CreateNodeByName(const std::string& name);
		void RemoveNode(std::shared_ptr<NNode> node);

		bool IsNodeSelected(NNode* node);
		void UpdateNodeConnectionsPoints(std::shared_ptr<NNode> node);

		void OnNodeDeleted(std::shared_ptr<NNode> node);
		void OnNodeDeselect();

		// Pin methods
		bool IsMouseHoveringPin(std::shared_ptr<NPin> pin);
		std::shared_ptr<NPin> GetMouseHoveredPin(std::shared_ptr<NNode> node);

	private:
		void SetCanvasData();
		void DrawCanvasGrid();

		float PointDistance(ImVec2 p1, ImVec2 p2);

	private:
		static bool m_Show;		// Easier use in menu if static - to toggle window visibility
		static bool m_ShowNodesPanel;
		static bool m_ShowDetailsPanel;
		static bool m_ShowPopup;

		NGraphController* m_GraphController = nullptr;
	};
}