#include "stpch.h"
#include "Gui/Nodes/GraphPanel.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/Project.h"
#include "Gui/EditorUtils.h"
#include "Gui/Components/UIPayload.h"
#include "Gui/Components/SearchPopup.h"
#include "Gui/Components/Toolbar.h"
#include "Gui/Components/ImGuiEx.h"
#include "Gui/Nodes/GraphSerializer.h"

#include "Gui/Nodes/NodeBuilder.h"
#include "Gui/CodeGen/ClassBuilder.h"

#include <glm/gtc/type_ptr.hpp>

namespace Stimpi
{
	bool GraphPanel::m_Show = false;
	bool GraphPanel::m_ShowNodesPanel = true;
	bool GraphPanel::m_ShowDetailsPanel = true;
	bool GraphPanel::m_ShowPopup = false;

	struct GraphPanelContext
	{
		GraphPanelCanvas m_Canvas;

		// Active open Graphs
		std::vector<std::shared_ptr<Graph>> m_Graphs;
		Graph* m_ActiveGraph = nullptr;	// actively edited graph (active tab)
		Variable* m_SelectedVariable = nullptr;

		ImDrawList* m_DrawList;

		bool m_IsHovered = false;
		bool m_IsActive = false;
		bool m_ZoomEnabled = true;

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

	// Pin data type specific ImGui input helpers
	static void VariableValueInput(Variable* var, int val)
	{
		std::string label = fmt::format("Int##{}", var->m_ID);
		ImGui::DragInt(label.c_str(), &val);
		var->m_Value = val;
	}
	
	static void VariableValueInput(Variable* var, bool val)
	{
		std::string label = fmt::format("Bool##{}", var->m_ID);
		ImGui::Checkbox(label.c_str(), &val);
		var->m_Value = val;
	}

	static void VariableValueInput(Variable* var, float val)
	{
		std::string label = fmt::format("Float##{}", var->m_ID);
		ImGui::DragFloat(label.c_str(), &val);
		var->m_Value = val;
	}
	
	static void VariableValueInput(Variable* var, glm::vec2 val)
	{
		std::string label = fmt::format("Vector2##{}", var->m_ID);
		ImGui::DragFloat2("label", glm::value_ptr(val));
		var->m_Value = val;
	}

	static void VariableValueInput(Variable* var, std::string val)
	{
		ImGui::Text("String support TBD");
	}

	static void DrawVariableNameInput(Variable* selected)
	{
		char nameInputBuff[32] = { "" };
		if (selected->m_Name.length() < 32)
		{
			strcpy_s(nameInputBuff, selected->m_Name.c_str());
		}

		if (ImGui::InputText("Variable Name##DetailsPanel", nameInputBuff, sizeof(nameInputBuff), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			selected->m_Name = std::string(nameInputBuff);

			// Since the text label changes we need to update node size
			for (auto& pin : selected->m_AttachedToPins)
			{
				pin->m_ParentNode->m_Size = CalcNodeSize(pin->m_ParentNode);
			}
		}
		EditorUtils::SetActiveItemCaptureKeyboard(false);
	}

	static void DrawVariableNodeTypeSelect(Variable* selected)
	{
		const char* variableTypeStrings[] = PIN_VALUE_TYPES_LIST;
		const char* currentVariableTypeStrings = variableTypeStrings[PIN_VALUE_TYPE_TO_INT(selected->m_ValueType)];

		if (ImGui::BeginCombo("Variable Type", currentVariableTypeStrings))
		{
			int s = IM_ARRAYSIZE(variableTypeStrings);
			for (int i = 0; i < IM_ARRAYSIZE(variableTypeStrings); i++)
			{
				bool isSelected = currentVariableTypeStrings == variableTypeStrings[i];
				if (ImGui::Selectable(variableTypeStrings[i], isSelected))
				{
					currentVariableTypeStrings = variableTypeStrings[i];
					UpdateVariableValueType(selected, INT_TO_PIN_VALUE_TYPE(i));
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	}

	/**
	 *  GraphPanel class code starts here
	 */

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

		//CreateNodeByName(ImVec2(150.0f, 150.0f), "OnCreate");
		//CreateNode(ImVec2(150.0f, 150.0f), "Test Node");
		//CreateNode(ImVec2(350.0f, 350.0f), "Another Node");
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
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
			ImGui::Begin("Node Panel", &m_Show, ImGuiWindowFlags_MenuBar);
			ImGui::PopStyleVar();

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			s_Context->m_DrawList = draw_list;
			m_GraphRenderer->SetDrawList(s_Context->m_DrawList);

			// Menu bar
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("View##GraphPanel"))
				{
					if (ImGui::MenuItem("Nodes", nullptr, m_ShowNodesPanel))
					{
						m_ShowNodesPanel = !m_ShowNodesPanel;
					}
					if (ImGui::MenuItem("Details", nullptr, m_ShowDetailsPanel))
					{
						m_ShowDetailsPanel = !m_ShowDetailsPanel;
					}
					ImGui::EndMenu();
				}

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


			if (ImGui::BeginTable("##GraphPanelTable", 3, ImGuiTableFlags_Resizable))
			{
				ImGui::TableSetupColumn("ColNodePanel", !m_ShowNodesPanel ? ImGuiTableColumnFlags_Disabled : 0);
				ImGui::TableSetupColumn("ColGraph");
				ImGui::TableSetupColumn("ColDetailsPanel", !m_ShowDetailsPanel ? ImGuiTableColumnFlags_Disabled : 0);

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

	void GraphPanel::DrawNodesPanel()
	{
		ImGui::BeginChild("NodesInspector##Child", ImVec2(0.0f, 0.0f), false);

		if (ImGui::BeginTabBar("NodesInspectorTabBar", ImGuiTabBarFlags_AutoSelectNewTabs))
		{
			if (ImGui::BeginTabItem("Graph_Name_here", &m_ShowNodesPanel, ImGuiTabItemFlags_None))
			{
				ImVec2 cursor = ImGui::GetCursorPos(); // For positioning AddButton icon
				if (ImGui::CollapsingHeader("Variables##NodesInspector", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
				{
					for (auto& variable : s_Context->m_ActiveGraph->m_Variables)
					{
						if (variable) // because removing of an item can happen while iterating collection
						{
							ImGui::PushID(variable->m_ID);
							if (ImGui::TreeNodeEx((void*)&variable, ImGuiTreeNodeFlags_Leaf, variable->m_Name.c_str()))
							{
								if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
								{
									s_Context->m_SelectedVariable = variable.get();
								}

								ItemRightClickPopup();

							}

							UIPayload::BeginSource(PAYLOAD_NODE_VARIABLE_GET, variable->m_Name.c_str(), variable->m_Name.length(), variable->m_Name.c_str());

							ImGui::PopID();
							ImGui::TreePop();
						}
					}
				}
				ImGui::Separator();

				AddItemPopupButton(cursor, "##AddNewVariable", []()
					{
						auto graph = s_Context->m_ActiveGraph;
						if (graph)
						{
							auto newVar = std::make_shared<Variable>(Variable::ValueType::Int);
							graph->m_Variables.emplace_back(newVar);
						}
					});
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::EndChild();
	}

	void GraphPanel::DrawDetailsPanel()
	{
		ImGui::BeginChild("Details##Child", ImVec2(0.0f, 0.0f), false);

		if (ImGui::BeginTabBar("DetailsTabBar", ImGuiTabBarFlags_AutoSelectNewTabs))
		{
			if (ImGui::BeginTabItem("Details", &m_ShowDetailsPanel, ImGuiTabItemFlags_None))
			{
				Variable* selected = nullptr;
				// Selected Node has priority for var details
				auto node = m_GraphController->GetSelectedNode();
				if (node)
				{
					if (node->m_Type == Node::NodeType::Variable)
					{
						selected = node->m_OutPins.front().get()->m_Variable.get();
					}
				}
				if (selected == nullptr)
					selected = s_Context->m_SelectedVariable;

				if (selected)
				{
					if (ImGui::CollapsingHeader("Variable##DetailsPanel", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (selected)
						{
							DrawVariableNameInput(selected);
							// We can assume that there is at most 1 outPin in VariableNodes
							DrawVariableNodeTypeSelect(selected);
						}
					}

					if (ImGui::CollapsingHeader("Default Value##DetailsPanel", ImGuiTreeNodeFlags_DefaultOpen))
					{
						if (selected)
						{
							// We can assume that there is at most 1 outPin in VariableNodes
							DrawVariableValueInput(selected);
						}
					}
				}
				ImGui::Separator();


				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}

		
		ImGui::EndChild();
	}

	void GraphPanel::DrawGraph()
	{
		// Main Toolbar
		Toolbar::Begin("NodePanelToolbar");
		{
			if (Toolbar::ToolbarButton("Compile"))
			{
				ST_CORE_INFO("Compile button presed");
				ClassBuilder::GenerateCode(s_Context->m_ActiveGraph, "TempGenClass.cs");
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

					// Handle drag-dropped nodes
					UIPayload::BeginTarget(PAYLOAD_NODE_VARIABLE_GET, [&](void* data, uint32_t size) {
						std::string strData = std::string((char*)data, size);
						ImVec2 mouseGraphPos = GetNodePanelViewMouseLocation();
						CreateNodeByName(mouseGraphPos, strData);
					});

					ImGui::EndTabItem();
				}
				toRemove = !graph->m_Show ? graph.get() : nullptr;
			}

			ImGui::EndTabBar();
		}
	}

	void GraphPanel::DrawGraphOverlay()
	{
		// Draw zoom level
		s_Context->m_DrawList->AddText(
			{ s_Context->m_Canvas.m_PosMax.x - 90, s_Context->m_Canvas.m_PosMin.y + 10 },
			IM_COL32(220, 220, 220, 255), fmt::format("Zoom 1:{:.2f}", 1.0f / s_Context->m_Canvas.m_Scale).c_str());
	}

	void GraphPanel::DrawVariableValueInput(Variable* variable)
	{
		std::visit([&variable](auto&& arg) { VariableValueInput(variable, arg); }, variable->m_Value);
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

	Graph* GraphPanel::GetGlobalActiveGraph()
	{
		return s_Context->m_ActiveGraph;
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
		pin->m_Variable->m_Name = "Something";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin);

		std::shared_ptr<Pin> pin2 = std::make_shared<Pin>();
		pin2->m_ID = ++s_Context->m_ActiveGraph->m_NextPinID;
		pin2->m_ParentNode = newNode.get();
		pin2->m_Variable->m_Name = "Else";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin2);

		std::shared_ptr<Pin> pin3 = std::make_shared<Pin>();
		pin3->m_ID = ++s_Context->m_ActiveGraph->m_NextPinID;
		pin3->m_ParentNode = newNode.get();
		pin3->m_Variable->m_Name = "Output";
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
			pin->m_Variable->m_Name = item.m_Variable.m_Name;
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
			newNode->m_Size = CalcNodeSize(newNode);

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

	void GraphPanel::PrepareRemoveNode(Node* node)
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
	}

	bool GraphPanel::IsNodeSelected(Node* node)
	{
		if (node != nullptr && m_GraphController->GetSelectedNode() != nullptr)
			return node->m_ID == m_GraphController->GetSelectedNode()->m_ID;

		return false;
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

		if (show)
		{
			s_Context->m_NewNodePos = GetNodePanelViewMouseLocation();
			m_ShowPopup = true;
			SearchPopup::OpenPopup();
		}

		if (m_ShowPopup)
		{
			std::vector<std::string>& nodeTypeList = NodeBuilder::GetNodeNamesList();

			if (SearchPopup::OnImGuiRender(nodeTypeList))
			{
				m_ShowPopup = false;
				CreateNodeByName(s_Context->m_NewNodePos, SearchPopup::GetSelection());
			}
		}
	}

	ImVec2 GraphPanel::GetNodePanelViewMouseLocation()
	{
		ImGuiIO& io = ImGui::GetIO();
		return { (io.MousePos.x - s_Context->m_Canvas.m_Origin.x) / s_Context->m_Canvas.m_Scale, (io.MousePos.y - s_Context->m_Canvas.m_Origin.y) / s_Context->m_Canvas.m_Scale };
	}

	bool GraphPanel::IsMouseHoveringConnection(PinConnection* connection)
	{
		ImVec2 mousePos = GetNodePanelViewMouseLocation();

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
		if (zoom <= 1.0f && zoom >= 0.09f && s_Context->m_ZoomEnabled)
			s_Context->m_Canvas.m_Scale = zoom;
	}

	void GraphPanel::SetZoomEnable(bool enable)
	{
		s_Context->m_ZoomEnabled = enable;
	}

	void GraphPanel::OnNodeDeleted(Node* deleted)
	{
		RemoveNode(deleted);

		// When a node gets deleted we clear lingering selections
		s_Context->m_SelectedVariable = nullptr;
	}

	void GraphPanel::OnNodeDeselect()
	{
		s_Context->m_SelectedVariable = nullptr;
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

	void GraphPanel::AddItemPopupButton(ImVec2 cursorPos, std::string name, std::function<void()> onClickAction)
	{
		// Save cursor position
		ImVec2 temp = ImGui::GetCursorPos();
		cursorPos.x += ImGui::GetWindowContentRegionWidth() - ImGuiEx::GetStyle().m_IconOffset * 1.4f;
		ImGui::SetCursorPos(cursorPos);
		static bool showSettings = false;

		std::string btnID = name.append("_IconButton");
		if (ImGuiEx::IconButton(btnID.c_str(), EDITOR_ICON_CROSS))
		{
			onClickAction();
		}

		// Restore cursor position
		ImGui::SetCursorPos(temp);
	}

	void GraphPanel::ItemRightClickPopup()
	{
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("ItemClickPopup##GraphPanel");
		}

		if (ImGui::BeginPopup("ItemClickPopup##GraphPanel"))
		{
			if (ImGui::Selectable("Delete"))
			{
				// Remove selected variable
				if (s_Context->m_SelectedVariable)
				{
					// Find Nodes using variable to delete
					auto iter = s_Context->m_ActiveGraph->m_Nodes.begin();
					while (iter != s_Context->m_ActiveGraph->m_Nodes.end())
					{
						auto node = *iter;
						// Only handle Variable type nodes
						if (node->m_Type == Node::NodeType::Variable || node->m_Type == Node::NodeType::Method)
						{
							if (s_Context->m_SelectedVariable->m_ID == node->m_OutPins.front()->m_Variable->m_ID)
							{
								PrepareRemoveNode(node.get());
								iter = s_Context->m_ActiveGraph->m_Nodes.erase(iter);
							}
							else
							{
								iter++;
							}
						}
						else
						{
							iter++;
						}
					}

					s_Context->m_ActiveGraph->RemoveVariable(s_Context->m_SelectedVariable);
					s_Context->m_SelectedVariable = nullptr;
				}
			}
			ImGui::EndPopup();
		}
	}

}