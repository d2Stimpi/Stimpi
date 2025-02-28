#include "stpch.h"
#include "Gui/NNode/NGraphPanel.h"

#include "Gui/NNode/NGraphRenderer.h"
#include "Gui/NNode/NGraphStyle.h"
#include "Gui/NNode/NGraphRegistry.h"
#include "Gui/NNode/NNodeRegistry.h"
#include "Gui/NNode/Exec/ExecTreeBuilder.h"
#include "Gui/NNode/NGraphSerializer.h"

#include "Stimpi/VisualScripting/ExecTree.h"

#include "Gui/Components/Toolbar.h"
#include "Gui/Components/SearchPopup.h"

//temp
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Core/Project.h"
#include "Stimpi/VisualScripting/ExecTreeSerializer.h"

namespace Stimpi
{

	bool NGraphPanel::m_Show = false;
	bool NGraphPanel::m_ShowNodesPanel = true;
	bool NGraphPanel::m_ShowDetailsPanel = true;
	bool NGraphPanel::m_ShowPopup = false;

	struct NGraphPanelContext
	{
		NGraphPanelCanvas m_Canvas;
		ImDrawList* m_DrawList;
		NGraphController* m_GraphController;

		bool m_IsHovered = false;
		bool m_IsActive = false;
		bool m_ZoomEnabled = true;

		// Popup data
		ImVec2 m_NewNodePos = { 0.0f, 0.0f };

		bool m_DebugTooltip = false;

		std::unordered_map<UUID, std::shared_ptr<NGraph>> m_Graphs;
		NGraph* m_ActiveGraph;

		// Temp execution tree
		std::shared_ptr<ExecTree> m_TempExecTree;
	};

	static NGraphPanelContext* s_Context;

	NGraphPanel::NGraphPanel()
	{
		NNodeRegistry::InitializeNodeRegisrty();

		s_Context = new NGraphPanelContext();

		NGraphRenderer::SetDrawCanvas(&s_Context->m_Canvas);
		NGraphRenderer::SetPanelContext(this);

		s_Context->m_GraphController = new NGraphController(this);
		s_Context->m_GraphController->SetDrawCanvas(&s_Context->m_Canvas);

		// Load available graphs from corresponding resource directory
		NGraphRegistry::PreloadExistingGraphs();

		AddGraph(std::make_shared<NGraph>());
	}

	NGraphPanel::~NGraphPanel()
	{
	}

	void NGraphPanel::SetDrawList(ImDrawList* drawList)
	{
		s_Context->m_DrawList = drawList;
		NGraphRenderer::SetDrawList(drawList);
	}

	void NGraphPanel::OnImGuiRender()
	{
		if (m_Show)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
			ImGui::Begin("Node Panel v2", &m_Show, ImGuiWindowFlags_MenuBar);
			ImGui::PopStyleVar();

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			s_Context->m_DrawList = draw_list;
			NGraphRenderer::SetDrawList(s_Context->m_DrawList);

			// Menu bar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("View##NGraphPanel"))
				{
					if (ImGui::MenuItem("Nodes##NGraphPanel", nullptr, m_ShowNodesPanel))
					{
						m_ShowNodesPanel = !m_ShowNodesPanel;
					}
					if (ImGui::MenuItem("Details##NGraphPanel", nullptr, m_ShowDetailsPanel))
					{
						m_ShowDetailsPanel = !m_ShowDetailsPanel;
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Menu##NGraphPanel"))
				{
					if (ImGui::MenuItem("Show Tooltip##NGraphPanel", nullptr, s_Context->m_DebugTooltip))
					{
						s_Context->m_DebugTooltip = !s_Context->m_DebugTooltip;
					}
					if (ImGui::MenuItem("Debug##NGraphPanel", nullptr, NGraphRenderer::IsDebugModeOn()))
					{
						NGraphRenderer::SetDebugMode(!NGraphRenderer::IsDebugModeOn());
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			// Main panel components
			if (ImGui::BeginTable("##NGraphPanelTable", 3, ImGuiTableFlags_Resizable))
			{
				ImGui::TableSetupColumn("ColNodePanel##NGraphPanelTable", !m_ShowNodesPanel ? ImGuiTableColumnFlags_Disabled : 0);
				ImGui::TableSetupColumn("ColGraph##NGraphPanelTable");
				ImGui::TableSetupColumn("ColDetailsPanel##NGraphPanelTable", !m_ShowDetailsPanel ? ImGuiTableColumnFlags_Disabled : 0);

				ImGui::TableNextColumn();
				if (m_ShowNodesPanel)
				{
					DrawNodesPanel();
				}

				ImGui::TableSetColumnIndex(1);
				DrawGraph();

				if (m_ShowDetailsPanel)
				{
					ImGui::TableSetColumnIndex(2);
					DrawDetailsPanel();
				}

				ImGui::EndTable();
			}

			ImGui::End();
		}
	}

	void NGraphPanel::DrawNodesPanel()
	{
		ImGui::BeginChild("NodesInspector##NGraphPanel", ImVec2(0.0f, 0.0f), false);

		if (ImGui::BeginTabBar("NodesInspectorTabBar##NGraphPanel", ImGuiTabBarFlags_AutoSelectNewTabs))
		{
			if (ImGui::BeginTabItem("Graph_Name_here", &m_ShowNodesPanel, ImGuiTabItemFlags_None))
			{
				ImVec2 cursor = ImGui::GetCursorPos(); // For positioning AddButton icon
				if (ImGui::CollapsingHeader("Variables##NGraphPanelNodesInspector", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
				{
					// Draw content here
				}
				ImGui::Separator();

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		ImGui::EndChild();
	}

	void NGraphPanel::DrawDetailsPanel()
	{
		ImGui::BeginChild("Details##NGraphPanel", ImVec2(0.0f, 0.0f), false);

		if (ImGui::BeginTabBar("DetailsTabBar##NGraphPanel", ImGuiTabBarFlags_AutoSelectNewTabs))
		{
			if (ImGui::BeginTabItem("Details##NGraphPanel", &m_ShowDetailsPanel, ImGuiTabItemFlags_None))
			{
				// Draw details here

				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}


		ImGui::EndChild();
	}

	void NGraphPanel::DrawGraph()
	{
		// Main Toolbar
		Toolbar::Begin("NodePanelToolbar##NGraphPanel");
		{
			if (Toolbar::ToolbarButton("Compile##NGraphPanel"))
			{
				ST_CORE_INFO("Compile button presed");
				s_Context->m_TempExecTree = ExecTreeBuilder::BuildExecutionTree(s_Context->m_ActiveGraph);
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarButton("TestExec##NGraphPanel"))
			{
				auto scene = SceneManager::Instance()->GetActiveScene();

				if (s_Context->m_TempExecTree)
				{
					//s_Context->m_TempExecTree->ExecuteWalk(scene->GetEntityByHandle((entt::entity)1));
					s_Context->m_TempExecTree->ExecuteWalk(scene->FindentityByName("Player"));
				}
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarButton("Save##NGraphPanel"))
			{
				if (s_Context->m_ActiveGraph)
				{
					NGraphSerializer serializer(s_Context->m_ActiveGraph);
					std::string fileName = s_Context->m_ActiveGraph->m_Name;
					FilePath path = Project::GetResourcesSubdir(Project::Subdir::VisualScripting) / fileName.append(".ngh");
					serializer.Serialize(path);
				}
				else
				{
					ST_INFO("[GraphPanel] No graph to save found!");
				}

				if (s_Context->m_TempExecTree && s_Context->m_ActiveGraph)
				{
					ExecTreeSerializer execTreeSerializer(s_Context->m_TempExecTree.get());
					std::string fileName = s_Context->m_ActiveGraph->m_Name;
					FilePath path = Project::GetResourcesSubdir(Project::Subdir::VisualScripting) / fileName.append(".egh");
					execTreeSerializer.Serialize(path);
				}
				else
				{
					ST_INFO("[GraphPanel] No compiled graph to save found!");
				}
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarButton("Load##NGraphPanel"))
			{
				NGraphSerializer serializer(s_Context->m_ActiveGraph);
				std::string fileName = s_Context->m_ActiveGraph->m_Name;
				FilePath path = Project::GetResourcesSubdir(Project::Subdir::VisualScripting) / fileName.append(".ngh");
				serializer.Deseriealize(path);

				s_Context->m_ActiveGraph->RegenerateGraphDataAfterLoad();

				if (s_Context->m_TempExecTree == nullptr)
					s_Context->m_TempExecTree = std::make_shared<ExecTree>();

				ExecTreeSerializer execTreeSerializer(s_Context->m_TempExecTree.get());
				Project::GetResourcesSubdir(Project::Subdir::VisualScripting) / fileName.append(".egh");
				execTreeSerializer.Deseriealize(path);
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarButton("Add Graph##NGraphPanel"))
			{

			}
			Toolbar::Separator();
		}
		Toolbar::End();

		// Tabs start here
		static ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;

		if (ImGui::BeginTabBar("NodePanelNewTabBar##NGraphPanel", tabBarFlags))
		{
			// Render all graphs as TabItems
			for (auto& item : s_Context->m_Graphs)
			{
				auto& graph = item.second;
				if (ImGui::BeginTabItem(graph->m_Name.c_str(), &graph->m_Show, ImGuiTabItemFlags_None))
				{
					//Set active graph to selected tab item
					s_Context->m_ActiveGraph = graph.get();

					SetCanvasData();

					// Process mouse control before any drawing calls
					s_Context->m_GraphController->UpdateMouseControls();
					s_Context->m_GraphController->HandleKeyPresses();

					DrawCanvasGrid();
					NGraphRenderer::OnImGuiRender();

					// Rect pushed in drawing of the Grid (start of the draw commands)
					s_Context->m_DrawList->PopClipRect();
					// Draw border
					s_Context->m_DrawList->AddRect(s_Context->m_Canvas.m_PosMin, s_Context->m_Canvas.m_PosMax, IM_COL32(255, 255, 255, 255));

					DrawGraphOverlay();

					ImGui::EndTabItem();
				}
			}

			ImGui::EndTabBar();
		}
	}

	void NGraphPanel::DrawGraphOverlay()
	{
		// Draw zoom level
		s_Context->m_DrawList->AddText(
			{ s_Context->m_Canvas.m_PosMax.x - 90, s_Context->m_Canvas.m_PosMin.y + 10 },
			IM_COL32(220, 220, 220, 255), fmt::format("Zoom 1:{:.2f}", 1.0f / s_Context->m_Canvas.m_Scale).c_str());
	}

	void NGraphPanel::ShowWindow(bool show)
	{
		m_Show = show;
	}

	void NGraphPanel::ShowGraph(const std::string& name, bool closeOther)
	{
		m_Show = true;

		// TODO:
	}

	void NGraphPanel::ShowGraph(std::shared_ptr<NGraph> graph, bool closeOther)
	{
		if (graph)
		{
			m_Show = true;
		
			if (closeOther)
				s_Context->m_Graphs.clear();

			AddGraph(graph);
			s_Context->m_GraphController->SetActiveGraph(s_Context->m_ActiveGraph);
		}
	}

	bool NGraphPanel::IsVisible()
	{
		return m_Show;
	}

	void NGraphPanel::AddNodePopup(bool show)
	{
		if (show)
		{
			s_Context->m_NewNodePos = GetNodePanelViewMouseLocation();
			m_ShowPopup = true;
			SearchPopup::OpenPopup("AddNodePopup##GraphPanel");
		}

		if (m_ShowPopup)
		{
			std::vector<std::string>& nodeTypeList = NNodeRegistry::GetNodeNamesList();

			if (SearchPopup::OnImGuiRender("AddNodePopup##GraphPanel", nodeTypeList))
			{
				m_ShowPopup = false;
				CreateNodeByName(SearchPopup::GetSelection());
			}
		}
	}

	ImVec2 NGraphPanel::GetNodePanelViewMouseLocation()
	{
		ImGuiIO& io = ImGui::GetIO();
		return { (io.MousePos.x - s_Context->m_Canvas.m_Origin.x) / s_Context->m_Canvas.m_Scale, (io.MousePos.y - s_Context->m_Canvas.m_Origin.y) / s_Context->m_Canvas.m_Scale };
	}

	void NGraphPanel::AddGraph(std::shared_ptr<NGraph> graph)
	{
		if (graph == nullptr)
			return;

		auto found = s_Context->m_Graphs.find(graph->m_ID);
		if (found == s_Context->m_Graphs.end())
			s_Context->m_Graphs[graph->m_ID] = graph;

		graph->m_Show = true;
		s_Context->m_ActiveGraph = graph.get();

		s_Context->m_GraphController->SetActiveGraph(s_Context->m_ActiveGraph);
	}

	void NGraphPanel::SetCanvasData()
	{
		s_Context->m_Canvas.m_PosMin = ImGui::GetCursorScreenPos();	   // ImDrawList API uses screen coordinates!
		s_Context->m_Canvas.m_Size = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
		if (s_Context->m_Canvas.m_Size.x < 50.0f) s_Context->m_Canvas.m_Size.x = 50.0f;
		if (s_Context->m_Canvas.m_Size.y < 50.0f) s_Context->m_Canvas.m_Size.y = 50.0f;
		s_Context->m_Canvas.m_PosMax = ImVec2(s_Context->m_Canvas.m_PosMin.x + s_Context->m_Canvas.m_Size.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Canvas.m_Size.y);

		ImGuiIO& io = ImGui::GetIO();
		ImGui::InvisibleButton("canvas", s_Context->m_Canvas.m_Size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
		s_Context->m_IsHovered = ImGui::IsItemHovered(); // Hovered
		s_Context->m_IsActive = ImGui::IsItemActive();   // Held

		s_Context->m_GraphController->SetActive(s_Context->m_IsActive);

		s_Context->m_Canvas.m_Origin = { s_Context->m_Canvas.m_PosMin.x + s_Context->m_Canvas.m_Scrolling.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Canvas.m_Scrolling.y };


		// Debug Tooltip
		if (s_Context->m_DebugTooltip)
		{
			if (ImGui::BeginItemTooltip())
			{
				ImGuiIO& io = ImGui::GetIO();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::Text("Canvas posMin: %f, %f", s_Context->m_Canvas.m_PosMin.x, s_Context->m_Canvas.m_PosMin.y);
				ImGui::Text("Canvas posMax: %f, %f", s_Context->m_Canvas.m_PosMax.x, s_Context->m_Canvas.m_PosMax.y);
				ImGui::Text("Mouse: %f, %f", io.MousePos.x, io.MousePos.y);
				ImVec2 viewPos = { io.MousePos.x - s_Context->m_Canvas.m_PosMin.x, io.MousePos.y - s_Context->m_Canvas.m_PosMin.y };
				ImGui::Text("=>: %f, %f", viewPos.x, viewPos.y);
				ImGui::Text("Origin: %f, %f", s_Context->m_Canvas.m_Origin.x, s_Context->m_Canvas.m_Origin.y);
				ImGui::Text("=>: %f, %f", io.MousePos.x - s_Context->m_Canvas.m_Origin.x, io.MousePos.y - s_Context->m_Canvas.m_Origin.y);
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}
		}
	}

	void NGraphPanel::DrawCanvasGrid()
	{
		ImVec2 canvasPosMin = s_Context->m_Canvas.m_PosMin;
		ImVec2 canvasPosMax = s_Context->m_Canvas.m_PosMax;
		ImVec2 canvasSize = s_Context->m_Canvas.m_Size;
		ImVec2 scrolling = s_Context->m_Canvas.m_Scrolling;

		// Background color
		s_Context->m_DrawList->AddRectFilled(canvasPosMin, canvasPosMax, IM_COL32(50, 50, 50, 255));

		// Draw grid + all lines in the canvas
		s_Context->m_DrawList->PushClipRect(canvasPosMin, canvasPosMax, true);
		{
			const float GRID_STEP = s_PanelStyle.m_GridStep * s_Context->m_Canvas.m_Scale;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvasSize.x; x += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x + x, canvasPosMin.y), ImVec2(canvasPosMin.x + x, canvasPosMax.y), IM_COL32(200, 200, 200, 40));
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvasSize.y; y += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x, canvasPosMin.y + y), ImVec2(canvasPosMax.x, canvasPosMin.y + y), IM_COL32(200, 200, 200, 40));
		}
	}

	float NGraphPanel::PointDistance(ImVec2 p1, ImVec2 p2)
	{
		return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	}

	NGraph* NGraphPanel::GetActiveGraph()
	{
		return s_Context->m_ActiveGraph;
	}

	NGraphController* NGraphPanel::GetController()
	{
		return s_Context->m_GraphController;
	}

	bool NGraphPanel::IsMouseHoverNode(NNode* node)
	{
		ImVec2 min(s_Context->m_Canvas.m_Origin.x + node->m_Pos.x * s_Context->m_Canvas.m_Scale, s_Context->m_Canvas.m_Origin.y + node->m_Pos.y * s_Context->m_Canvas.m_Scale);
		ImVec2 max(min.x + node->m_Size.x * s_Context->m_Canvas.m_Scale, min.y + node->m_Size.y * s_Context->m_Canvas.m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	NNodeId NGraphPanel::GetMouseHoverNode()
	{
		for (auto& node : s_Context->m_ActiveGraph->m_Nodes)
		{
			if (IsMouseHoverNode(node.get()))
			{
				return node->m_ID;
			}
		}

		return 0; // 0 - no hovered node
	}

	bool NGraphPanel::IsMouseHoveringConnection(std::shared_ptr<NPinConnection> connection)
	{
		ImVec2 mousePos = GetNodePanelViewMouseLocation();

		for (auto it = connection->m_BezierLinePoints.begin(); std::next(it) != connection->m_BezierLinePoints.end(); it++)
		{
			if (std::next(it) != connection->m_BezierLinePoints.end())
			{
				ImVec2 p1 = *it;
				ImVec2 p2 = *(std::next(it));

				if (PointDistance(p1, mousePos) + PointDistance(mousePos, p2) <= PointDistance(p1, p2) + 1.0f)
				{
					return true;
				}
			}
		}

		return false;
	}

	std::shared_ptr<Stimpi::NPinConnection> NGraphPanel::GetMouseHoveredConnection()
	{
		for (auto& connection : s_Context->m_ActiveGraph->m_PinConnections)
		{
			if (IsMouseHoveringConnection(connection))
				return connection;
		}

		return nullptr;
	}

	std::shared_ptr<NNode> NGraphPanel::GetNodeByID(NNodeId id)
	{
		for (auto& node : s_Context->m_ActiveGraph->m_Nodes)
		{
			if (node->m_ID == id)
				return node;
		}

		return nullptr;
	}

	float NGraphPanel::GetPanelZoom()
	{
		return s_Context->m_Canvas.m_Scale;
	}

	void NGraphPanel::SetPanelZoom(float zoom)
	{
		if (zoom <= 1.0f && zoom >= 0.09f && s_Context->m_ZoomEnabled)
			s_Context->m_Canvas.m_Scale = zoom;
	}

	void NGraphPanel::SetZoomEnable(bool enable)
	{
		s_Context->m_ZoomEnabled = enable;
	}

	void NGraphPanel::CreateNodeByName(const std::string& name)
	{
		auto newNode = NNodeRegistry::CreateNodeByName(name);
		if (newNode)
		{
			newNode->m_Pos = s_Context->m_NewNodePos;

			s_Context->m_ActiveGraph->m_Nodes.push_back(newNode);
		}
	}

	void NGraphPanel::RemoveNode(std::shared_ptr<NNode> node)
	{
		if (!node)
			return;

		// Break all connections to the Node
		for (auto inPin : node->m_InPins)
		{
			for (auto cPin : inPin->m_ConnectedPins)
			{
				auto connection = s_Context->m_ActiveGraph->FindPinToPinConnection(inPin, cPin);
				s_Context->m_ActiveGraph->BreakPinToPinConnection(connection);
			}
		}

		for (auto outPin : node->m_OutPins)
		{
			for (auto cPin : outPin->m_ConnectedPins)
			{
				auto connection = s_Context->m_ActiveGraph->FindPinToPinConnection(outPin, cPin);
				s_Context->m_ActiveGraph->BreakPinToPinConnection(connection);
			}
		}

		// Remove Node
		auto& nodes = s_Context->m_ActiveGraph->m_Nodes;
		auto found = std::find(nodes.begin(), nodes.end(), node);
		if (found != nodes.end())
			nodes.erase(found);
	}

	void NGraphPanel::OnNodeDeleted(std::shared_ptr<NNode> node)
	{
		RemoveNode(node);
	}

	void NGraphPanel::OnNodeDeselect()
	{
		// TBD
	}

	bool NGraphPanel::IsMouseHoveringPin(std::shared_ptr<NPin> pin)
	{
		ImVec2 min(s_Context->m_Canvas.m_Origin.x + (pin->m_Pos.x - s_PanelStyle.m_PinRadius - s_PanelStyle.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale, s_Context->m_Canvas.m_Origin.y + (pin->m_Pos.y - s_PanelStyle.m_PinRadius - s_PanelStyle.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale);
		ImVec2 max(s_Context->m_Canvas.m_Origin.x + (pin->m_Pos.x + s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale, s_Context->m_Canvas.m_Origin.y + (pin->m_Pos.y + s_PanelStyle.m_PinRadius + s_PanelStyle.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	std::shared_ptr<Stimpi::NPin> NGraphPanel::GetMouseHoveredPin(std::shared_ptr<NNode> node)
	{
		if (node == nullptr)
			return nullptr;

		for (auto& pin : node->m_InPins)
		{
			if (IsMouseHoveringPin(pin))
				return pin;
		}

		for (auto& pin : node->m_OutPins)
		{
			if (IsMouseHoveringPin(pin))
				return pin;
		}

		return nullptr;
	}

	bool NGraphPanel::IsNodeSelected(NNode* node)
	{
		if (node != nullptr && s_Context->m_GraphController->GetSelectedNode() != nullptr)
			return node->m_ID == s_Context->m_GraphController->GetSelectedNode()->m_ID;

		return false;
	}

	void NGraphPanel::UpdateNodeConnectionsPoints(std::shared_ptr<NNode> node)
	{
		auto selected = s_Context->m_GraphController->GetSelectedNode();
		for (auto& inPin : selected->m_InPins)
		{
			if (inPin->m_Connected)
			{
				for (auto connPin : inPin->m_ConnectedPins)
				{
					auto connection = s_Context->m_ActiveGraph->FindPinToPinConnection(inPin, connPin);
					if (connection)
						connection->UpdateConnectionPoints();
				}
			}
		}

		for (auto& outPin : selected->m_OutPins)
		{
			if (outPin->m_Connected)
			{
				for (auto connPin : outPin->m_ConnectedPins)
				{
					auto connection = s_Context->m_ActiveGraph->FindPinToPinConnection(outPin, connPin);
					if (connection)
						connection->UpdateConnectionPoints();
				}
			}
		}
	}

}