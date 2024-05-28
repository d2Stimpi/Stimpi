#include "stpch.h"
#include "Gui/Nodes/GraphPanel.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"
#include "Gui/Components/SearchPopup.h"
#include "Gui/Components/Toolbar.h"
#include "Gui/Nodes/GraphSerializer.h"

#include "Gui/Nodes/NodeBuilder.h"

namespace Stimpi
{
	bool GraphPanel::m_Show = false;

	struct GraphPanelContext
	{
		GraphPanelCanvas m_Canvas;

		// Active open Graphs
		std::vector<std::shared_ptr<Graph>> m_Graphs;
		Graph* m_ActiveGraph = nullptr;	// actively edited graph (active tab)

		ImDrawList* m_DrawList;

		bool m_IsHovered = false;
		bool m_IsActive = false;

		Node* m_SelectedNode = nullptr; // TODO: remove from here
		// Popup data
		ImVec2 m_NewNodePos = { 0.0f, 0.0f };

		bool m_DebugTooltip = false;
	};

	static GraphPanelContext* s_Context;

	static void InitNodePanelStyle()
	{
		s_Style.m_HeaderHeight = 30.0f;
		s_Style.m_HeaderTextOffset = { 35.0f, 8.0f };
		s_Style.m_NodeHoverColor = IM_COL32(220, 220, 30, 255);

		s_Style.m_NodeRounding = 15.0f;
		s_Style.m_SelectionThinckness = 3.0f;

		float pinScale = 0.7f;
		s_Style.m_PinOffset = 10.0f * pinScale; // Offset from Node frame side
		s_Style.m_PinRadius = 8.0f * pinScale;
		s_Style.m_PinThickness = 3.0f * pinScale;
		s_Style.m_PinArrowSpacing = 2.0f * pinScale;
		s_Style.m_PinArrowHalfHeight = 4.0f * pinScale;
		s_Style.m_PinArrowWidth = 4.0f * pinScale;
		s_Style.m_PinSpacing = 14.0f;
		s_Style.m_PinTextSpacing = 4.0f;
		s_Style.m_PinSelectOffset = 4.0f;

		s_Style.m_ConnectionSegments = 20;

		s_Style.m_LineThickness = 3.0f;

		s_Style.m_GridStep = 16.0f;
	}

	GraphPanel::GraphPanel()
	{
		s_Context = new GraphPanelContext();

		m_GraphRenderer = new GraphRenderer(this);
		m_GraphController = new GraphContorller(this);

		m_GraphRenderer->SetDrawCanvas(&s_Context->m_Canvas);
		m_GraphController->SetDrawCanvas(&s_Context->m_Canvas);

		InitNodePanelStyle();

		// Populate node Atlas with data
		NodeBuilder::InitializeNodeList();

		AddGraph(new Graph());

		CreateNode(ImVec2(150.0f, 150.0f), "Test Node");
		CreateNode(ImVec2(350.0f, 350.0f), "Another Node");
	}

	GraphPanel::~GraphPanel()
	{
		delete s_Context;
	}

	void GraphPanel::SetDrawList(ImDrawList* drawList)
	{
		s_Context->m_DrawList = drawList;
		m_GraphRenderer->SetDrawList(drawList);
	}

	void GraphPanel::OnImGuiRender()
	{
		if (m_Show)
		{
			ImGui::Begin("Node Panel", &m_Show, ImGuiWindowFlags_MenuBar);

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			s_Context->m_DrawList = draw_list;
			m_GraphRenderer->SetDrawList(s_Context->m_DrawList);

			// Menu bar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Menu##GraphPanel"))
				{
					if (ImGui::MenuItem("Show Tooltip", nullptr, s_Context->m_DebugTooltip))
					{
						s_Context->m_DebugTooltip = !s_Context->m_DebugTooltip;
					}
					if (ImGui::MenuItem("Debug", nullptr, m_GraphRenderer->IsDebugModeOn()))
					{
						m_GraphRenderer->SetDebugMode(!m_GraphRenderer->IsDebugModeOn());
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}

			// Main Toolbar
			Toolbar::Begin("NodePanelToolbar");
			{
				if (Toolbar::ToolbarButton("Compile"))
				{
					ST_CORE_INFO("Compile button presed");
				}
				Toolbar::Separator();

				if (Toolbar::ToolbarButton("Save"))
				{
					if (s_Context->m_ActiveGraph)
					{
						GraphSerializer serializer(s_Context->m_ActiveGraph);
						FilePath path = Project::GetProjectDir() / "Graph.txt";
						serializer.Serialize(path);
					}
				}
				Toolbar::Separator();

				if (Toolbar::ToolbarButton("Load"))
				{
					GraphSerializer serializer(s_Context->m_ActiveGraph);
					FilePath path = Project::GetProjectDir() / "Graph.txt";
					serializer.Deseriealize(path);

					RegenerateGraphDataAfterLoad(s_Context->m_ActiveGraph);
				}
				Toolbar::Separator();

				if (Toolbar::ToolbarButton("Add Graph"))
				{
					static int graphNameCnt = 0;
					std::string name = fmt::format("Graph {}", graphNameCnt++);
					AddGraph(new Graph(name));
				}
				Toolbar::Separator();
			}
			Toolbar::End();

			// Tabs start here
			static ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;

			if (ImGui::BeginTabBar("NodePanelNewTabBar", tabBarFlags))
			{
				Graph* toRemove = nullptr; // Deffer removing of "closed" graph from the list after iterating trough it
				// Each graph rendering goes here
				for (auto& graph : s_Context->m_Graphs)
				{
					if (ImGui::BeginTabItem(graph->m_Name.c_str(), &graph->m_Show, ImGuiTabItemFlags_None))
					{
						//Set active graph to selected tab item
						s_Context->m_ActiveGraph = graph.get();

						SetCanvasData();

						// Process mouse control before any drawing calls
						m_GraphController->UpdateMouseControls();
						m_GraphController->HandleKeyPresses();

						DrawCanvasGrid();
						m_GraphRenderer->OnImGuiRender();

						// Rect pushed in drawing of the Grid (start of the draw commands)
						s_Context->m_DrawList->PopClipRect();
						// Draw border
						s_Context->m_DrawList->AddRect(s_Context->m_Canvas.m_PosMin, s_Context->m_Canvas.m_PosMax, IM_COL32(255, 255, 255, 255));

						DrawGraphOverlay();

						ImGui::EndTabItem();
					}
					toRemove = !graph->m_Show ? graph.get() : nullptr;
				}

				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}

	void GraphPanel::DrawGraphOverlay()
	{
		// Draw zoom level
		s_Context->m_DrawList->AddText(
			{ s_Context->m_Canvas.m_PosMax.x - 90, s_Context->m_Canvas.m_PosMin.y + 10 },
			IM_COL32(220, 220, 220, 255), fmt::format("Zoom 1:{:.2f}", 1.0f / s_Context->m_Canvas.m_Scale).c_str());
	}

	void GraphPanel::AddGraph(Graph* graph)
	{
		if (graph == nullptr)
			return;

		for (auto& item : s_Context->m_Graphs)
		{
			if (item->m_Name == graph->m_Name)
				return;
		}

		graph->m_Show = true;
		s_Context->m_ActiveGraph = graph;
		s_Context->m_Graphs.emplace_back(graph);

		m_GraphController->SetActiveGraph(s_Context->m_ActiveGraph);
	}

	void GraphPanel::RemoveGraph(Graph* graph)
	{
		if (graph == nullptr)
			return;

		ST_CORE_INFO("Removing graph \"{}\"", graph->m_Name);

		s_Context->m_Graphs.erase(std::remove_if(s_Context->m_Graphs.begin(), s_Context->m_Graphs.end(),
			[&graph](std::shared_ptr<Graph> cmp) { return cmp->m_Name == graph->m_Name; }));

		ST_CORE_INFO("New total graph count {}", s_Context->m_Graphs.size());

		// When removing the Tab Item - Graph, new selection is set to the last added Graph
		// - This will most likely get overridden by the selected Tab Item
		if (!s_Context->m_Graphs.empty())
			s_Context->m_ActiveGraph = s_Context->m_Graphs[s_Context->m_Graphs.size() - 1].get();
		else
			s_Context->m_ActiveGraph = nullptr;

		m_GraphController->SetActiveGraph(s_Context->m_ActiveGraph);
	}

	Stimpi::Graph* GraphPanel::GetActiveGraph()
	{
		return s_Context->m_ActiveGraph;
	}

	void GraphPanel::ShowWindow(bool show)
	{
		m_Show = show;
	}

	bool GraphPanel::IsVisible()
	{
		return m_Show;
	}

	void GraphPanel::CreateNode(ImVec2 pos, std::string title)
	{
		std::shared_ptr<Node> newNode = std::make_shared<Node>();
		newNode->m_Pos = pos;
		newNode->m_Size = { 250.0f, 100.0f };
		newNode->m_ID = ++s_Context->m_ActiveGraph->m_NextNodeID;
		newNode->m_Title = title;

		// Add some pins for testing
		std::shared_ptr<Pin> pin = std::make_shared<Pin>();
		pin->m_ID = ++s_Context->m_ActiveGraph->m_NextPinID;
		pin->m_ParentNode = newNode.get();
		pin->m_Text = "Something";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin);

		std::shared_ptr<Pin> pin2 = std::make_shared<Pin>();
		pin2->m_ID = ++s_Context->m_ActiveGraph->m_NextPinID;
		pin2->m_ParentNode = newNode.get();
		pin2->m_Text = "Else";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin2);

		std::shared_ptr<Pin> pin3 = std::make_shared<Pin>();
		pin3->m_ID = ++s_Context->m_ActiveGraph->m_NextPinID;
		pin3->m_ParentNode = newNode.get();
		pin3->m_Text = "Output";
		pin3->m_Type = Pin::Type::OUTPUT;
		newNode->m_OutPins.emplace_back(pin3);

		s_Context->m_ActiveGraph->m_Nodes.emplace_back(newNode);
	}

	void GraphPanel::CreateNode(ImVec2 pos, std::string title, NodeLayout layout)
	{
		std::shared_ptr<Node> newNode = std::make_shared<Node>();
		newNode->m_Pos = pos;
		newNode->m_Size = { 250.0f, 100.0f };
		newNode->m_ID = s_Context->m_ActiveGraph->GenerateNodeID();
		newNode->m_Title = title;

		for (auto& item : layout)
		{
			std::shared_ptr<Pin> pin = std::make_shared<Pin>();
			pin->m_ID = s_Context->m_ActiveGraph->GeneratePinID();
			pin->m_ParentNode = newNode.get();
			pin->m_Text = item.m_Text;
			pin->m_Type = item.m_Type;
			newNode->m_InPins.emplace_back(pin);
		}

		s_Context->m_ActiveGraph->m_Nodes.emplace_back(newNode);
	}

	void GraphPanel::CreateNodeByName(ImVec2 pos, std::string title)
	{
		Node* newNode = NodeBuilder::CreateNodeByName(title, s_Context->m_ActiveGraph);
		if (newNode)
		{
			newNode->m_Pos = pos;

			s_Context->m_ActiveGraph->m_Nodes.emplace_back(newNode);
		}
	}

	void GraphPanel::RemoveNode(Node* node)
	{
		if (!node)
			return;

		// Break all connections to the Node
		for (auto inPin : node->m_InPins)
		{
			for (auto cPin : inPin->m_ConnectedPins)
			{
				BreakPinToPinConnection(FindPinToPinConnection(inPin.get(), cPin, s_Context->m_ActiveGraph), s_Context->m_ActiveGraph);
			}
		}

		for (auto outPin : node->m_OutPins)
		{
			for (auto cPin : outPin->m_ConnectedPins)
			{
				BreakPinToPinConnection(FindPinToPinConnection(outPin.get(), cPin, s_Context->m_ActiveGraph), s_Context->m_ActiveGraph);
			}
		}

		// Remove Node
		for (auto it = s_Context->m_ActiveGraph->m_Nodes.begin(); it != s_Context->m_ActiveGraph->m_Nodes.end();)
		{
			Node* remNode = (*it).get();
			if (remNode->m_ID == node->m_ID)
			{
				s_Context->m_ActiveGraph->m_Nodes.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	bool GraphPanel::IsNodeSelected(Node* node)
	{
		if (node != nullptr && m_GraphController->GetSelectedNode() != nullptr)
			return node->m_ID == m_GraphController->GetSelectedNode()->m_ID;

		return false;
	}

	void GraphPanel::SetSelectedNode(Node* node)
	{
		s_Context->m_SelectedNode = node;
	}

	void GraphPanel::UpdateNodeConnectionsPoints(Node* node)
	{
		Node* selected = m_GraphController->GetSelectedNode();
		for (auto inPin : selected->m_InPins)
		{
			if (inPin->m_Connected)
			{
				for (auto connPin : inPin->m_ConnectedPins)
				{
					PinConnection* connection = FindPinToPinConnection(inPin.get(), connPin, s_Context->m_ActiveGraph);
					if (connection)
						UpdateConnectionPoints(connection);
				}
			}
		}

		for (auto outPin : selected->m_OutPins)
		{
			if (outPin->m_Connected)
			{
				for (auto connPin : outPin->m_ConnectedPins)
				{
					PinConnection* connection = FindPinToPinConnection(outPin.get(), connPin, s_Context->m_ActiveGraph);
					if (connection)
						UpdateConnectionPoints(connection);
				}
			}
		}
	}

	void GraphPanel::AddNodePopup(bool show)
	{
		static bool showPopup = false;

		if (show)
		{
			s_Context->m_NewNodePos = GetNodePanelViewClickLocation();
			showPopup = true;
			SearchPopup::OpenPopup();
		}

		if (showPopup)
		{
			//static std::vector<std::string> nodeTypeList = { "New node" };
			std::vector<std::string>& nodeTypeList = NodeBuilder::GetNodeNamesList();

			if (SearchPopup::OnImGuiRender(nodeTypeList))
			{
				showPopup = false;
				//CreateNode(s_Context->m_NewNodePos, SearchPopup::GetSelection());
				CreateNodeByName(s_Context->m_NewNodePos, SearchPopup::GetSelection());
			}
		}
	}

	ImVec2 GraphPanel::GetNodePanelViewClickLocation()
	{
		ImGuiIO& io = ImGui::GetIO();
		return { (io.MousePos.x - s_Context->m_Canvas.m_Origin.x) / s_Context->m_Canvas.m_Scale, (io.MousePos.y - s_Context->m_Canvas.m_Origin.y) / s_Context->m_Canvas.m_Scale };
	}

	bool GraphPanel::IsMouseHoveringConnection(PinConnection* connection)
	{
		ImVec2 mousePos = GetNodePanelViewClickLocation();

		for (auto it = connection->m_BezierLinePoints.begin(); std::next(it) != connection->m_BezierLinePoints.end(); it++)
		{
			ImVec2 p1 = *it;
			ImVec2 p2 = *(std::next(it));

			if (PointDistance(p1, mousePos) + PointDistance(mousePos, p2) <= PointDistance(p1, p2) + 1.0f)
			{
				return true;
			}
		}

		return false;
	}

	Stimpi::PinConnection* GraphPanel::GetMouseHoveredConnection()
	{
		for (auto& connection : s_Context->m_ActiveGraph->m_PinConnections)
		{
			if (IsMouseHoveringConnection(connection.get()))
				return connection.get();
		}

		return nullptr;
	}

	bool GraphPanel::IsMouseHoverNode(Node* node)
	{
		ImVec2 min(s_Context->m_Canvas.m_Origin.x + node->m_Pos.x * s_Context->m_Canvas.m_Scale, s_Context->m_Canvas.m_Origin.y + node->m_Pos.y * s_Context->m_Canvas.m_Scale);
		ImVec2 max(min.x + node->m_Size.x * s_Context->m_Canvas.m_Scale, min.y + node->m_Size.y * s_Context->m_Canvas.m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	uint32_t GraphPanel::GetMouseHoverNode()
	{
		for (auto node : s_Context->m_ActiveGraph->m_Nodes)
		{
			if (IsMouseHoverNode(node.get()))
			{
				return node->m_ID;
			}
		}

		return 0; // 0 - no hovered node
	}

	Stimpi::Node* GraphPanel::GetNodeByID(uint32_t id)
	{
		for (auto& node : s_Context->m_ActiveGraph->m_Nodes)
		{
			if (node->m_ID == id)
				return node.get();
		}

		return nullptr;
	}

	float GraphPanel::GetPanelZoom()
	{
		return s_Context->m_Canvas.m_Scale;
	}

	void GraphPanel::SetPanelZoom(float zoom)
	{
		if (zoom <= 1.0f && zoom >= 0.09f)
			s_Context->m_Canvas.m_Scale = zoom;
	}

	bool GraphPanel::IsMouseHoveringPin(Pin* pin)
	{
		ImVec2 min(s_Context->m_Canvas.m_Origin.x + (pin->m_Pos.x - s_Style.m_PinRadius - s_Style.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale, s_Context->m_Canvas.m_Origin.y + (pin->m_Pos.y - s_Style.m_PinRadius - s_Style.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale);
		ImVec2 max(s_Context->m_Canvas.m_Origin.x + (pin->m_Pos.x + s_Style.m_PinRadius + s_Style.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale, s_Context->m_Canvas.m_Origin.y + (pin->m_Pos.y + s_Style.m_PinRadius + s_Style.m_PinSelectOffset) * s_Context->m_Canvas.m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	Stimpi::Pin* GraphPanel::GetMouseHoveredPin(Node* node)
	{
		if (node == nullptr)
			return nullptr;

		for (auto& pin : node->m_InPins)
		{
			if (IsMouseHoveringPin(pin.get()))
				return pin.get();
		}

		for (auto& pin : node->m_OutPins)
		{
			if (IsMouseHoveringPin(pin.get()))
				return pin.get();
		}

		return nullptr;
	}

	void GraphPanel::SetCanvasData()
	{
		s_Context->m_Canvas.m_PosMin = ImGui::GetCursorScreenPos();	   // ImDrawList API uses screen coordinates!
		s_Context->m_Canvas.m_Size = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
		if (s_Context->m_Canvas.m_Size.x < 50.0f) s_Context->m_Canvas.m_Size.x = 50.0f;
		if (s_Context->m_Canvas.m_Size.y < 50.0f) s_Context->m_Canvas.m_Size.y = 50.0f;
		s_Context->m_Canvas.m_PosMax = ImVec2(s_Context->m_Canvas.m_PosMin.x + s_Context->m_Canvas.m_Size.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Canvas.m_Size.y);

		ImGuiIO& io = ImGui::GetIO();
		ImGui::InvisibleButton("canvas", s_Context->m_Canvas.m_Size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		s_Context->m_IsHovered = ImGui::IsItemHovered(); // Hovered
		s_Context->m_IsActive = ImGui::IsItemActive();   // Held

		m_GraphController->SetActive(s_Context->m_IsActive);

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

	void GraphPanel::DrawCanvasGrid()
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
			const float GRID_STEP = s_Style.m_GridStep * s_Context->m_Canvas.m_Scale;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvasSize.x; x += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x + x, canvasPosMin.y), ImVec2(canvasPosMin.x + x, canvasPosMax.y), IM_COL32(200, 200, 200, 40));
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvasSize.y; y += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x, canvasPosMin.y + y), ImVec2(canvasPosMax.x, canvasPosMin.y + y), IM_COL32(200, 200, 200, 40));
		}
	}
}