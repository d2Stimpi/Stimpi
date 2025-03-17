#pragma once

#include "Gui/NNode/NGraphController.h"
#include "Gui/NNode/NGraph.h"

#include "Stimpi/VisualScripting/ExecTree.h"

namespace Stimpi
{
	using OnGraphCompiledListener = std::function<void(std::shared_ptr<ExecTree> execTree)>;

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
		void OnClose();

		void DrawInspectorPanel();
		void DrawDetailsPanel();
		void DrawGraph();
		void DrawGraphOverlay();

		static void SetOnGraphCompiledListener(OnGraphCompiledListener listener);
		static void ShowWindow(bool show);
		static void ShowGraph(const std::string& name, bool closeOther = false);
		static void ShowGraph(std::shared_ptr<NGraph> graph, bool closeOther = false);
		static bool IsVisible();

		void AddNodePopup(bool show);
		ImVec2 GetNodePanelViewMouseLocation();

		// Graph methods
		static void AddGraph(std::shared_ptr<NGraph> graph);
		static void RemoveGraph(UUID graphID);
		NGraph* GetActiveGraph();

		NGraphController* GetController();

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

		void AddItemPopupButton(ImVec2 cursorPos, std::string name, std::function<void()> onClickAction);
		void ItemRightClickPopup();
		void ProcessCollectionRemovals();

		void BuildNodesList();

	private:
		static bool m_Show;		// Easier use in menu if static - to toggle window visibility
		static bool m_ShowInspectorPanel;
		static bool m_ShowDetailsPanel;
		static bool m_ShowPopup;
	};
}