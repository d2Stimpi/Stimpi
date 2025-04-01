#include "stpch.h"
#include "Gui/NNode/NGraphPanel.h"

#include "Gui/NNode/NGraphRenderer.h"
#include "Gui/NNode/NGraphStyle.h"
#include "Gui/NNode/NGraphRegistry.h"
#include "Gui/NNode/NNodeRegistry.h"
#include "Gui/NNode/Exec/ExecTreeBuilder.h"
#include "Gui/NNode/NGraphSerializer.h"

#include "Gui/Components/Toolbar.h"
#include "Gui/Components/SearchPopup.h"

#include "ImGui/src/imgui_internal.h"

//temp
#include "Stimpi/Scene/SceneManager.h"
#include "Stimpi/Core/Project.h"
//#include "Stimpi/VisualScripting/ExecTreeSerializer.h"
#include "Stimpi/VisualScripting/ExecTreeRegistry.h"


namespace Stimpi
{

	bool NGraphPanel::m_Show = false;
	bool NGraphPanel::m_ShowInspectorPanel = true;
	bool NGraphPanel::m_ShowDetailsPanel = true;
	bool NGraphPanel::m_ShowPopup = false;

	OnGraphCompiledListener s_OnGraphCompiledListener = nullptr;

	enum class SelectionType { None = 0, Graph, Variable, LocalVariable };

	struct NGraphPanelContext
	{
		NGraphPanelCanvas m_Canvas;
		ImDrawList* m_DrawList = nullptr;
		NGraphController* m_GraphController = nullptr;

		bool m_IsHovered = false;
		bool m_IsActive = false;
		bool m_ZoomEnabled = true;

		// Popup data
		ImVec2 m_NewNodePos = { 0.0f, 0.0f };

		bool m_DebugTooltip = false;

		std::unordered_map<UUID, std::shared_ptr<NGraph>> m_Graphs;
		NGraph* m_ActiveGraph = nullptr;

		// Temp execution tree
		std::shared_ptr<ExecTree> m_TempExecTree;

		// Tab tracking helper data
		ImGuiTabBar* m_TabBar = nullptr;

		// Inspector panel data
		SelectionType m_SelectionType = SelectionType::None;
		NGraph* m_SelectedGraph = nullptr;
		std::vector<UUID> m_GraphsToBeRemoved;
	};

	static NGraphPanelContext* s_Context;

	static std::vector<std::string> s_NodeNames;

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

		BuildNodesList();
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
		static bool wasShowing = false;

		if (m_Show)
		{
			wasShowing = true;
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
					if (ImGui::MenuItem("Inspector##NGraphPanel", nullptr, m_ShowInspectorPanel))
					{
						m_ShowInspectorPanel = !m_ShowInspectorPanel;
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
				ImGui::TableSetupColumn("ColInspectorPanel##NGraphPanelTable", !m_ShowInspectorPanel ? ImGuiTableColumnFlags_Disabled : 0);
				ImGui::TableSetupColumn("ColGraph##NGraphPanelTable");
				ImGui::TableSetupColumn("ColDetailsPanel##NGraphPanelTable", !m_ShowDetailsPanel ? ImGuiTableColumnFlags_Disabled : 0);

				ImGui::TableNextColumn();
				if (m_ShowInspectorPanel)
				{
					DrawInspectorPanel();
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

		if (wasShowing && !m_Show)
		{
			OnClose();
		}
		wasShowing = m_Show;
	}

	void NGraphPanel::OnClose()
	{
		// If GraphPanel is closed, clear OnGraphCompiledListener
		s_OnGraphCompiledListener = nullptr;
	}

	ImGuiTabBar* s_tabbar;

	void NGraphPanel::DrawInspectorPanel()
	{
		ImGui::BeginChild("NodesInspector##NGraphPanel", ImVec2(0.0f, 0.0f), false);

		if (ImGui::BeginTabBar("NodesInspectorTabBar##NGraphPanel", ImGuiTabBarFlags_AutoSelectNewTabs))
		{
			if (ImGui::BeginTabItem("Inspector##NGraphPanel", &m_ShowInspectorPanel, ImGuiTabItemFlags_None))
			{
				ImVec2 cursor; // For positioning AddButton icon

				// Process item removals before iterating collections
				ProcessCollectionRemovals();

				// Graphs

				cursor = ImGui::GetCursorPos();
				if (ImGui::CollapsingHeader("Graphs##NGraphPanelNodesInspector", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
				{
					auto& graphs = NGraphRegistry::GetData();
					for (auto& item : graphs)
					{
						auto graph = item.second;
						ImGui::PushID(&graph->m_ID);
						if (ImGui::TreeNodeEx((void*)&graph->m_ID, ImGuiTreeNodeFlags_Leaf, graph->m_Name.c_str()))
						{
							if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
							{
								s_Context->m_SelectedGraph = graph.get();
								s_Context->m_SelectionType = SelectionType::Graph;

								if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
								{
									ShowGraph(graph, false);
									graph->m_Show = true;
									// Try to set it as active / visible
									ImGuiWindow* rootWindow = ImGui::GetCurrentWindow()->RootWindow;
									if (s_Context->m_TabBar && ImGui::TabBarFindTabByID(s_Context->m_TabBar, graph->m_ImGuiId) != NULL)
									{
										s_Context->m_TabBar->SelectedTabId = graph->m_ImGuiId;
									}
								}
							}

							ItemRightClickPopup();
						}

						ImGui::PopID();
						ImGui::TreePop();
					}
				}
				ImGui::Separator();

				AddItemPopupButton(cursor, "##AddNewGraph", []()
					{
						std::shared_ptr<NGraph> newGraph = std::make_shared<NGraph>();
						AddGraph(newGraph);

						NGraphRegistry::RegisterGraph(newGraph);
					});

				// Variables

				cursor = ImGui::GetCursorPos();
				if (ImGui::CollapsingHeader("Variables##NGraphPanelNodesInspector", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
				{
					// Draw content here
				}
				ImGui::Separator();

				AddItemPopupButton(cursor, "##AddNewVariable", []()
					{
						
					});


				// Local Variables

				cursor = ImGui::GetCursorPos();
				if (ImGui::CollapsingHeader("Local Variables##NGraphPanelNodesInspector", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap))
				{
					// Draw content here
				}
				ImGui::Separator();

				AddItemPopupButton(cursor, "##AddNewLocalVariable", []()
					{

					});

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
		Toolbar::PushStyle({ 2.0f, ImVec2{35.0f, 35.0f}, ImVec2{32.0f, 32.0f} });
		Toolbar::Begin("NodePanelToolbar##NGraphPanel");
		{
			bool recompileGraph = false;
			if (s_Context->m_ActiveGraph)
				recompileGraph = s_Context->m_ActiveGraph->m_Recompile;

			if (Toolbar::ToolbarIconButton("##Compile##NGraphPanel", recompileGraph ? EDITOR_ICON_COMPILE_WRN : EDITOR_ICON_COMPILE))
			{
				ST_CORE_INFO("Compile button presed");
				s_Context->m_TempExecTree = ExecTreeBuilder::BuildExecutionTree(s_Context->m_ActiveGraph);

				// TODO: verify that compilation was successful
				if (s_OnGraphCompiledListener)
				{
					ExecTreeRegistry::RegisterExecTree(s_Context->m_ActiveGraph->m_ID, s_Context->m_TempExecTree);
					s_OnGraphCompiledListener(s_Context->m_TempExecTree);
				}

				// Mark that the graph was compiled
				s_Context->m_ActiveGraph->m_Recompile = false;
			}
			Toolbar::SetButtonTooltip(recompileGraph ? "Graph not compiled!" : "Compile");
			Toolbar::Separator();

			if (Toolbar::ToolbarButton("TestExec##NGraphPanel"))
			{
				auto scene = SceneManager::Instance()->GetActiveScene();

				if (s_Context->m_TempExecTree)
				{
					s_Context->m_TempExecTree->ExecuteWalk(scene->FindentityByName("Player"));
				}
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarIconButton("##Save##NGraphPanel", EDITOR_ICON_SAVE))
			{
				if (s_Context->m_ActiveGraph)
				{
					NGraphRegistry::SaveGraph(s_Context->m_ActiveGraph);
				}
				else
				{
					ST_INFO("[GraphPanel] No graph to save found!");
				}
			}
			Toolbar::SetButtonTooltip("Save");
			Toolbar::Separator();

			if (Toolbar::ToolbarButton("Load##NGraphPanel"))
			{
				if (s_Context->m_ActiveGraph)
				{
					std::string fileName = fmt::format("{}.egh", s_Context->m_ActiveGraph->m_ID);

					NGraphRegistry::LoadGraph(s_Context->m_ActiveGraph);
					s_Context->m_ActiveGraph->RegenerateGraphDataAfterLoad();
				}
			}
			Toolbar::Separator();

			if (Toolbar::ToolbarButton("Add Graph##NGraphPanel"))
			{

			}
			Toolbar::Separator();
		}
		Toolbar::End();
		Toolbar::PopStyle();

		// Tabs start here
		static ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;

		if (ImGui::BeginTabBar("NodePanelNewTabBar##NGraphPanel", tabBarFlags))
		{
			s_Context->m_TabBar = ImGui::GetCurrentTabBar();
			s_tabbar = s_Context->m_TabBar;

			// Render all graphs as TabItems
			auto& bla = s_Context->m_Graphs;
			for (auto& item : s_Context->m_Graphs)
			{
				auto& graph = item.second;
				if (graph != nullptr)
				{
					if (ImGui::BeginTabItem(graph->GetLabel().c_str(), &graph->m_Show, ImGuiTabItemFlags_None))
					{
						graph->m_ImGuiId = ImGui::GetItemID();

						ImGuiWindow* rootWindow = ImGui::GetCurrentWindow();
						if (s_Context->m_TabBar && ImGui::TabBarFindTabByID(s_Context->m_TabBar, graph->m_ImGuiId) != NULL)
							s_Context->m_TabBar->VisibleTabId = graph->m_ImGuiId;

						//Set active graph to selected tab item
						s_Context->m_ActiveGraph = graph.get();
						s_Context->m_GraphController->SetActiveGraph(s_Context->m_ActiveGraph);

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
			}

			ImGui::EndTabBar();
		}
	}

	void NGraphPanel::DrawGraphOverlay()
	{
		// Draw zoom level
		s_Context->m_DrawList->AddText(
			{ s_Context->m_Canvas.m_PosMax.x - 90, s_Context->m_Canvas.m_PosMin.y + 10.0f },
			IM_COL32(220, 220, 220, 255), fmt::format("Zoom 1:{:.2f}", 1.0f / s_Context->m_Canvas.m_Scale).c_str());

		// Debug data
		if (s_Context->m_DebugTooltip)
		{
			ImVec2 pos = { s_Context->m_Canvas.m_PosMin.x + 10.0f , s_Context->m_Canvas.m_PosMin.y + 10.0f };

			s_Context->m_DrawList->AddText(pos, IM_COL32(220, 220, 220, 255), "Debug Tooltip: ON");
			pos.y += ImGui::GetFontSize();
			s_Context->m_DrawList->AddText(pos, IM_COL32(220, 220, 220, 255), fmt::format("Controller Action: {}", (uint32_t)s_Context->m_GraphController->GetAction()).c_str());
			pos.y += ImGui::GetFontSize();
			s_Context->m_DrawList->AddText(pos, IM_COL32(220, 220, 220, 255), fmt::format("Pop-up: {}", m_ShowPopup).c_str());
		}
	}

	void NGraphPanel::SetOnGraphCompiledListener(OnGraphCompiledListener listener)
	{
		s_OnGraphCompiledListener = listener;
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
			// Will update data if something changed
			BuildNodesList();

			if (SearchPopup::OnImGuiRender("AddNodePopup##GraphPanel", s_NodeNames))
			{
				m_ShowPopup = false;
				CreateNodeByName(SearchPopup::GetSelection());
				SetZoomEnable(true);
			}
		}

		if (m_ShowPopup)
			SetZoomEnable(!SearchPopup::IsActive());
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

		s_Context->m_ActiveGraph = graph.get();

		s_Context->m_GraphController->SetActiveGraph(s_Context->m_ActiveGraph);
	}

	void NGraphPanel::RemoveGraph(UUID graphID)
	{
		auto found = s_Context->m_Graphs.find(graphID);
		if (found != s_Context->m_Graphs.end())
		{
			s_Context->m_Graphs.erase(found);
		}
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

	void NGraphPanel::AddItemPopupButton(ImVec2 cursorPos, std::string name, std::function<void()> onClickAction)
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

	void NGraphPanel::ItemRightClickPopup()
	{
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("ItemClickPopup##NGraphPanel");
		}

		if (ImGui::BeginPopup("ItemClickPopup##NGraphPanel"))
		{
			if (ImGui::Selectable("Delete"))
			{
				// Remove selected item
				if (s_Context->m_SelectionType == SelectionType::Graph)
				{
					s_Context->m_GraphsToBeRemoved.push_back(s_Context->m_SelectedGraph->m_ID);
				}
			}
			ImGui::EndPopup();
		}
	}

	void NGraphPanel::ProcessCollectionRemovals()
	{
		for (auto& uuid : s_Context->m_GraphsToBeRemoved)
		{
			NGraphRegistry::UnregisterGraph(uuid);
			RemoveGraph(uuid);
		}

		s_Context->m_GraphsToBeRemoved.clear();
	}

	void NGraphPanel::BuildNodesList()
	{
		s_NodeNames.clear();

		// Separator - Standard nodes
		s_NodeNames.emplace_back("#Standard");
		for(auto name : NNodeRegistry::GetNodeNamesList())
			s_NodeNames.push_back(name);

		// Separator - Material nodes
		s_NodeNames.emplace_back("#Materials");
		s_NodeNames.emplace_back("Mat_Tiling");
		s_NodeNames.emplace_back("Mat_Something");
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