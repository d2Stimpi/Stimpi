#include "stpch.h"
#include "Gui/Nodes/NodePanel.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Core/Project.h"

#include "Gui/Components/SearchPopup.h"
#include "Gui/Components/Toolbar.h"
#include "Gui/Nodes/GraphSerializer.h"

#include "ImGui/src/imgui_internal.h"

#include <cmath>

#define NODE_HOVER_COLOR	IM_COL32(220, 220, 30, 255)

#define CANVAS_MOVE_BUTTON ImGuiMouseButton_Left

namespace Stimpi
{
	bool s_Show = false;	// Easier use in menu if static - to toggle window visibility

	enum class ControllAction { NONE = 0, NODE_HOVER, NODE_DRAGABLE, SHOW_POPUP_ONRELEASE, CANVAS_MOVE, NODE_PIN_DRAG, CONNECTION_ONPRESS };

	/*struct NodePanelStyle
	{
		// Node header
		float m_HeaderHeight;
		ImVec2 m_HeaderTextOffset;

		// Node frame
		float m_NodeRounding;
		float m_SelectionThinckness;

		// Pin
		float m_PinOffset;
		float m_PinRadius;
		float m_PinThickness;
		float m_PinArrowSpacing;
		float m_PinArrowHalfHeight;
		float m_PinArrowWidth;
		float m_PinSpacing;
		float m_PinTextSpacing; // For OUT pins
		float m_PinSelectOffset; // For higher Pin select precision

		// Connection
		uint32_t m_ConnectionSegments;

		// Grid
		float m_GridStep;
	};*/

	struct NodeCanvas
	{
		ImVec2 m_PosMin = ImVec2(0.0f, 0.0f);
		ImVec2 m_PosMax = ImVec2(0.0f, 0.0f);
		ImVec2 m_Size = ImVec2(0.0f, 0.0f);

		NodeCanvas() = default;
	};

	struct NodePanelContext
	{
		// Active open Graphs
		std::vector<std::shared_ptr<Graph>> m_Graphs;
		Graph* m_ActiveGraph = nullptr;	// actively edited graph (active tab)

		ImVec2 m_Origin = { 0.0f, 0.0f };	// Global panel view translate offset
		ImVec2 m_Scrolling = { 0.0f, 0.0f };
		NodeCanvas m_Canvas;
		float m_Scale = 1.0f;

		bool m_IsHovered = false;
		bool m_IsActive = false;

		// Convenience to avoid iterating Nodes vector
		Node* m_SelectedNode = nullptr;
		Pin* m_SelectedPin = nullptr;
		PinConnection* m_SelectedConnection = nullptr;

		// Pin drag mouse position (draw Pin - Floating point line)
		ImVec2 m_PinFloatingTarget = { 0.0f, 0.0f };

		ImDrawList* m_DrawList;

		// Controls
		ControllAction m_Action = ControllAction::NONE;

		// Popup data
		ImVec2 m_NewNodePos = { 0.0f, 0.0f };

		// Debug
		bool m_DebugOn = false;
	};

	static NodePanelContext* s_Context;
	//static NodePanelStyle s_Style;

	/**
	 *  NodePanelStyle
	 */

	static void InitNodePanelStyle()
	{
		s_Style.m_HeaderHeight = 30.0f;
		s_Style.m_HeaderTextOffset = { 35.0f, 8.0f };

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

		s_Style.m_GridStep = 16.0f;
	}

	/**
	 * Some forwards, just move along
	 */

	static void CalculateBezierPoints(PinConnection* connection, Pin* src, Pin* dest, uint32_t segments);
	static void UpdateConnectionPoints(PinConnection* connection);

	/**
	 * Helper Utility functions
	 */
#pragma region Utility

	static ImVec2 GetNodePanelViewClickLocation()
	{
		ImGuiIO& io = ImGui::GetIO();
		return { (io.MousePos.x - s_Context->m_Origin.x) / s_Context->m_Scale, (io.MousePos.y - s_Context->m_Origin.y) / s_Context->m_Scale };
	}

	static float PointDistance(ImVec2 p1, ImVec2 p2)
	{
		return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	}

	static void RegenerateGraphDataAfterLoad(Graph* graph)
	{
		for (auto connection : graph->m_PinConnections)
		{
			UpdateConnectionPoints(connection.get());
		}
	}

#pragma endregion Utility

	/**
	 * Helper Node static functions
	 */
#pragma region Node

	static bool IsNodeSelected(Node* node)
	{
		if (node != nullptr && s_Context->m_SelectedNode != nullptr)
			return node->m_ID == s_Context->m_SelectedNode->m_ID;

		return false;
	}

	// Return Node's space (without header).
	static ImVec2 GetAvailableNodeSpaceSize(Node* node)
	{
		ImVec2 available = node->m_Size;

		available.y -= s_Style.m_HeaderHeight;

		return available;
	}

	// Return Node's extra size. Including header size and any other style added sizes.
	static ImVec2 GetNodeExtraSize(Node* node)
	{
		ImVec2 space = GetAvailableNodeSpaceSize(node);

		return { node->m_Size.x - space.x, node->m_Size.y - space.y };
	}

	// Check if Node's space can hold certain size.
	static bool IsNodeSpaceSizeAvailable(Node* node, ImVec2 size)
	{
		ImVec2 available = GetAvailableNodeSpaceSize(node);

		return (available.x >= size.x) && (available.y >= size.y);
	}

	// Resize the Node's space lacking axis size, account for header sizes, etc.
	// Return
	//   true - if any axis was resized
	static bool ResizeNodeSpace(Node* node, ImVec2 size)
	{
		bool resized = false;
		ImVec2 newSize = size;
		ImVec2 extra = GetNodeExtraSize(node);

		newSize.x += extra.x;
		newSize.y += extra.y;

		if (node->m_Size.x < newSize.x)
		{
			node->m_Size.x = newSize.x;
			resized = true;
		}

		if (node->m_Size.y < newSize.y)
		{
			node->m_Size.y = newSize.y;
			resized = true;
		}

		return true;
	}
#pragma endregion Node

#pragma region Pin
	/**
	 * Helper Pin static functions
	 */

	static float GetPinSpaceHeight()
	{
		return s_Style.m_PinRadius * 2.0f;
	}

	static float GetPinSpaceWidth()
	{
		return s_Style.m_PinRadius * 2.0f + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth;
	}

	static bool IsMouseHoveringPin(Pin* pin)
	{
		ImVec2 min(s_Context->m_Origin.x + (pin->m_Pos.x - s_Style.m_PinRadius - s_Style.m_PinSelectOffset) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y - s_Style.m_PinRadius - s_Style.m_PinSelectOffset) * s_Context->m_Scale);
		ImVec2 max(s_Context->m_Origin.x + (pin->m_Pos.x + s_Style.m_PinRadius + s_Style.m_PinSelectOffset) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y + s_Style.m_PinRadius + s_Style.m_PinSelectOffset) * s_Context->m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	static Pin* GetMouseHoveredPin(Node* node)
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

	static bool IsConnected(Pin* src, Pin* dest)
	{
		if (!src->m_Connected || !dest->m_Connected)
			return false;

		for (auto connected : src->m_ConnectedPins)
		{
			if ((connected->m_ParentNode->m_ID == dest->m_ParentNode->m_ID)	// check id parent node matches
				&& (connected->m_ID == dest->m_ID))
			{
				return true;
			}
		}

		return false;
	}

	static void ConnectPinToPin(Pin* src, Pin* dest)
	{
		if (IsConnected(src, dest) == false)
		{
			src->m_Connected = true;
			dest->m_Connected = true;

			src->m_ConnectedPins.emplace_back(dest);
			dest->m_ConnectedPins.emplace_back(src);

			// Global pin to pin connections
			std::shared_ptr<PinConnection> newConnection = std::make_shared<PinConnection>(src, dest);
			CalculateBezierPoints(newConnection.get(), src, dest, s_Style.m_ConnectionSegments);
			s_Context->m_ActiveGraph->m_PinConnections.emplace_back(newConnection);
		}
	}

	static void BreakPinToPinConnection(PinConnection* connection)
	{
		if (!connection)
			return;

		// Delete p1 and p2 mutual reference
		Pin* p1 = connection->m_SourcePin;
		Pin* p2 = connection->m_DestinationPin;

		p1->m_ConnectedPins.erase(std::remove(p1->m_ConnectedPins.begin(), p1->m_ConnectedPins.end(), p2));
		if (p1->m_ConnectedPins.empty())
			p1->m_Connected = false;

		p2->m_ConnectedPins.erase(std::remove(p2->m_ConnectedPins.begin(), p2->m_ConnectedPins.end(), p1));
		if (p2->m_ConnectedPins.empty())
			p2->m_Connected = false;

		// Remove from vector
		for (auto it = s_Context->m_ActiveGraph->m_PinConnections.begin(); it != s_Context->m_ActiveGraph->m_PinConnections.end();)
		{
			auto itConn = (*it).get();
			if (itConn == connection)
			{
				s_Context->m_ActiveGraph->m_PinConnections.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	static void UpdateConnectionPoints(PinConnection* connection)
	{
		if (connection)
		{
			connection->m_BezierLinePoints.clear();
			CalculateBezierPoints(connection, connection->m_SourcePin, connection->m_DestinationPin, s_Style.m_ConnectionSegments);
		}
	}

	static PinConnection* FindPinToPinConnection(Pin* src, Pin* dest)
	{
		if (src == nullptr || dest == nullptr)
			return nullptr;

		for (auto& connection : s_Context->m_ActiveGraph->m_PinConnections)
		{
			if (connection->m_SourcePin == src && connection->m_DestinationPin == dest)
				return connection.get();

			if (connection->m_SourcePin == dest && connection->m_DestinationPin == src)
				return connection.get();
		}

		return nullptr;
	}

#pragma endregion Pin

	/**
	 *  Helper Connection static functions
	 */
#pragma region Connection

	static ImVec2 CalcFirstMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, start.y };
	}

	static ImVec2 CalcLastMidBezierPoint(const ImVec2& start, const ImVec2& end)
	{
		return { start.x + (end.x - start.x) / 2.0f, end.y };
	}

	/* Based on ImGui's ImBezierCubicCalc */
	static ImVec2 BezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t)
	{
		float u = 1.0f - t;
		float w1 = u * u * u;
		float w2 = 3 * u * u * t;
		float w3 = 3 * u * t * t;
		float w4 = t * t * t;
		return ImVec2(w1 * p1.x + w2 * p2.x + w3 * p3.x + w4 * p4.x, w1 * p1.y + w2 * p2.y + w3 * p3.y + w4 * p4.y);
	}

	static void CalculateBezierPoints(PinConnection* connection, Pin* src, Pin* dest, uint32_t segments)
	{
		ImVec2 startPoint = src->m_Pos;
		ImVec2 endPoint = dest->m_Pos;
		ImVec2 middlePoint1 = CalcFirstMidBezierPoint(startPoint, endPoint);
		ImVec2 middlePoint2 = CalcLastMidBezierPoint(startPoint, endPoint);

		connection->m_BezierLinePoints.push_back(startPoint);

		float t_step = 1.0f / (float)segments;
		for (int i_step = 1; i_step <= segments; i_step++)
			connection->m_BezierLinePoints.push_back(BezierCubicCalc(startPoint, middlePoint1, middlePoint2, endPoint, t_step * i_step));
	}

	static void UpdateNodeConnectionsPoints(Node* node)
	{
		Node* selected = s_Context->m_SelectedNode;
		for (auto inPin : selected->m_InPins)
		{
			if (inPin->m_Connected)
			{
				for (auto connPin : inPin->m_ConnectedPins)
				{
					PinConnection* connection = FindPinToPinConnection(inPin.get(), connPin);
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
					PinConnection* connection = FindPinToPinConnection(outPin.get(), connPin);
					if (connection)
						UpdateConnectionPoints(connection);
				}
			}
		}
	}

	static bool IsMouseHoveringConnection(PinConnection* connection)
	{
		ImVec2 mousePos = GetNodePanelViewClickLocation();

		for (auto it = connection->m_BezierLinePoints.begin(); std::next(it) != connection->m_BezierLinePoints.end(); it++)
		{
			ImVec2 p1 = *it;
			ImVec2 p2 = *(std::next(it));

			/*float d1 = PointDistance(p1, mousePos);
			float d2 = PointDistance(mousePos, p2);
			float d3 = PointDistance(p1, p2);*/

			if (PointDistance(p1, mousePos) + PointDistance(mousePos, p2) <= PointDistance(p1, p2) + 1.0f)
			{
				return true;
			}
		}

		return false;
	}

	static PinConnection* GetMouseHoveredConnection()
	{
		for (auto& connection : s_Context->m_ActiveGraph->m_PinConnections)
		{
			if (IsMouseHoveringConnection(connection.get()))
				return connection.get();
		}

		return nullptr;
	}

#pragma endregion Connection

	/**
	 *  NodePanel Class
	 */
#pragma region NodePanelClass

	NodePanel::NodePanel()
	{
		m_HeaderImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/Gradient2.png");
		s_Context = new NodePanelContext();

		InitNodePanelStyle();

		AddGraph(new Graph());

		CreateNode(ImVec2(150.0f, 150.0f), "Test Node");
		CreateNode(ImVec2(350.0f, 350.0f), "Another Node");
	}

	NodePanel::~NodePanel()
	{
		AssetManager::Release(m_HeaderImage);
		delete s_Context;
	}

	void NodePanel::SetDrawList(ImDrawList* drawList)
	{
		s_Context->m_DrawList = drawList;
	}

	void NodePanel::OnImGuiRender()
	{
		if (s_Show)
		{
			ImGui::Begin("Test Node Panel", &s_Show);

			static float scale = 1.0f;
			ImGui::DragFloat("Panel scale", &scale, 0.01f, 0.02f, 10.0f);
			ImGui::SameLine();
			static bool dbgTooltip = false;
			ImGui::Checkbox("Debug Tooltip", &dbgTooltip);
			ImGui::SameLine();
			ImGui::Checkbox("Debug", &s_Context->m_DebugOn);
			s_Context->m_Scale = scale;

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

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			s_Context->m_DrawList = draw_list;

			// Tabs start here
			static ImGuiTabBarFlags tabBarFlags = ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs;

			if (ImGui::BeginTabBar("NodePanelTabBar", tabBarFlags))
			{
				Graph* toRemove = nullptr;	// Deffer removing of "closed" graph from the list after iterating trough it
				// Each graph rendering goes here
				for (auto& graph : s_Context->m_Graphs)
				{
					if (ImGui::BeginTabItem(graph->m_Name.c_str(), &graph->m_Show, ImGuiTabItemFlags_None))
					{
						//Set active graph to selected tab item
						s_Context->m_ActiveGraph = graph.get();

						s_Context->m_Canvas.m_PosMin = ImGui::GetCursorScreenPos();	   // ImDrawList API uses screen coordinates!
						s_Context->m_Canvas.m_Size = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
						if (s_Context->m_Canvas.m_Size.x < 50.0f) s_Context->m_Canvas.m_Size.x = 50.0f;
						if (s_Context->m_Canvas.m_Size.y < 50.0f) s_Context->m_Canvas.m_Size.y = 50.0f;
						s_Context->m_Canvas.m_PosMax = ImVec2(s_Context->m_Canvas.m_PosMin.x + s_Context->m_Canvas.m_Size.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Canvas.m_Size.y);

						ImGuiIO& io = ImGui::GetIO();
						ImGui::InvisibleButton("canvas", s_Context->m_Canvas.m_Size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
						s_Context->m_IsHovered = ImGui::IsItemHovered(); // Hovered
						s_Context->m_IsActive = ImGui::IsItemActive();   // Held

						s_Context->m_Origin = { s_Context->m_Canvas.m_PosMin.x + s_Context->m_Scrolling.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Scrolling.y };

						if (dbgTooltip)
						{
							if (ImGui::BeginItemTooltip())
							{
								ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
								ImGui::Text("Pos: %f, %f", s_Context->m_Canvas.m_PosMin.x, s_Context->m_Canvas.m_PosMin.y);
								ImGui::Text("Mouse: %f, %f", io.MousePos.x, io.MousePos.y);
								ImVec2 viewPos = { io.MousePos.x - s_Context->m_Canvas.m_PosMin.x, io.MousePos.y - s_Context->m_Canvas.m_PosMin.y };
								ImGui::Text("=>: %f, %f", viewPos.x, viewPos.y);
								ImGui::Text("Origin: %f, %f", s_Context->m_Origin.x, s_Context->m_Origin.y);
								ImGui::Text("=>: %f, %f", io.MousePos.x - s_Context->m_Origin.x, io.MousePos.y - s_Context->m_Origin.y);
								ImGui::PopTextWrapPos();
								ImGui::EndTooltip();
							}
						}

						// Process mouse control before any drawing calls
						UpdateMouseControls();
						HandleKeyPresses();

						DrawCanvasGrid();

						// Draw all nodes
						for (auto node : s_Context->m_ActiveGraph->m_Nodes)
						{
							DrawNode(node.get());
						}

						// Temp
						if (s_Context->m_SelectedConnection)
						{
							DrawPinToPinConnection(s_Context->m_SelectedConnection->m_SourcePin, s_Context->m_SelectedConnection->m_DestinationPin, NODE_HOVER_COLOR);
						}

						// Draw Debug stuff in the end
						for (auto connection : s_Context->m_ActiveGraph->m_PinConnections)
						{
							DbgDrawConnectionLinePoints(connection.get());
						}

						// Rect pushed in drawing of the Grid (start of the draw commands)
						s_Context->m_DrawList->PopClipRect();

						// Draw border
						s_Context->m_DrawList->AddRect(s_Context->m_Canvas.m_PosMin, s_Context->m_Canvas.m_PosMax, IM_COL32(255, 255, 255, 255));

						ImGui::EndTabItem();
					}
					toRemove = !graph->m_Show ? graph.get() : nullptr;
				}

				// Remove the closed TabItem content - Graph
				RemoveGraph(toRemove);

				ImGui::EndTabBar();
			}

			ImGui::End();
		}
	}

	void NodePanel::AddGraph(Graph* graph)
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
	}

	void NodePanel::RemoveGraph(Graph* graph)
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
	}	

	void NodePanel::ShowWindow(bool show)
	{
		s_Show = show;
	}

	bool NodePanel::IsVisible()
	{
		return s_Show;
	}

	void NodePanel::DrawCanvasGrid()
	{
		ImVec2 canvasPosMin = s_Context->m_Canvas.m_PosMin;
		ImVec2 canvasPosMax= s_Context->m_Canvas.m_PosMax;
		ImVec2 canvasSize = s_Context->m_Canvas.m_Size;
		ImVec2 scrolling = s_Context->m_Scrolling;

		// Background color
		s_Context->m_DrawList->AddRectFilled(canvasPosMin, canvasPosMax, IM_COL32(50, 50, 50, 255));

		// Draw grid + all lines in the canvas
		s_Context->m_DrawList->PushClipRect(canvasPosMin, canvasPosMax, true);
		{
			const float GRID_STEP = s_Style.m_GridStep * s_Context->m_Scale;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvasSize.x; x += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x + x, canvasPosMin.y), ImVec2(canvasPosMin.x + x, canvasPosMax.y), IM_COL32(200, 200, 200, 40));
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvasSize.y; y += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x, canvasPosMin.y + y), ImVec2(canvasPosMax.x, canvasPosMin.y + y), IM_COL32(200, 200, 200, 40));
		}
	}

	/**
	 *  Node methods
	 */

	void NodePanel::CreateNode(ImVec2 pos, std::string title)
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
		pin->m_Variable->m_Text = "Something";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin);

		std::shared_ptr<Pin> pin2 = std::make_shared<Pin>();
		pin2->m_ID = ++s_Context->m_ActiveGraph->m_NextPinID;
		pin2->m_ParentNode = newNode.get();
		pin2->m_Variable->m_Text = "Else";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin2);

		std::shared_ptr<Pin> pin3 = std::make_shared<Pin>();
		pin3->m_ID = ++s_Context->m_ActiveGraph->m_NextPinID;
		pin3->m_ParentNode = newNode.get();
		pin3->m_Variable->m_Text = "Output";
		pin3->m_Type = Pin::Type::OUTPUT;
		newNode->m_OutPins.emplace_back(pin3);

		s_Context->m_ActiveGraph->m_Nodes.emplace_back(newNode);
	}

	void NodePanel::RemoveNode(Node* node)
	{
		if (!node)
			return;

		// Break all connections to the Node
		for (auto inPin : node->m_InPins)
		{
			for (auto cPin : inPin->m_ConnectedPins)
			{
				BreakPinToPinConnection(FindPinToPinConnection(inPin.get(), cPin));
			}
		}

		for (auto outPin : node->m_OutPins)
		{
			for (auto cPin : outPin->m_ConnectedPins)
			{
				BreakPinToPinConnection(FindPinToPinConnection(outPin.get(), cPin));
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

	void NodePanel::DrawNode(Node* node)
	{
		ImVec2 nodePos = node->m_Pos;

		// TODO: move it elsewhere, so its not calculated each frame
		// Calculate size of all Node's pins for drawing Node Rect
		ImVec2 pinSpaceSize = {0.0f /*TODO: widht*/, 40.0f};
		// Check if Node can hold Pin space
		if (!IsNodeSpaceSizeAvailable(node, pinSpaceSize))
		{
			// Can't fit, resize node
			ResizeNodeSpace(node, pinSpaceSize);
		}

		s_Context->m_DrawList->AddRectFilled(
			{ s_Context->m_Origin.x + nodePos.x * s_Context->m_Scale, s_Context->m_Origin.y + nodePos.y * s_Context->m_Scale },
			{ s_Context->m_Origin.x + (nodePos.x + node->m_Size.x) * s_Context->m_Scale, s_Context->m_Origin.y + (nodePos.y + node->m_Size.y) * s_Context->m_Scale },
			IM_COL32(20, 23, 20, 255),
			15.0f * s_Context->m_Scale);

		ImVec2 uv_min = ImVec2(0.0f, 1.0f);
		ImVec2 uv_max = ImVec2(1.0f, 0.0f);
		auto texture = AssetManager::GetAsset(m_HeaderImage).As<Texture>();
		if (texture->Loaded())
		{
			s_Context->m_DrawList->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
				{ s_Context->m_Origin.x + nodePos.x * s_Context->m_Scale, s_Context->m_Origin.y + nodePos.y * s_Context->m_Scale },
				{ s_Context->m_Origin.x + (nodePos.x + node->m_Size.x) * s_Context->m_Scale, s_Context->m_Origin.y + (nodePos.y + s_Style.m_HeaderHeight) * s_Context->m_Scale },
				uv_min, uv_max,
				IM_COL32(225, 30, 30, 255),
				s_Style.m_NodeRounding * s_Context->m_Scale, ImDrawFlags_RoundCornersTop);
		}

		ImGui::SetWindowFontScale(s_Context->m_Scale);
		s_Context->m_DrawList->AddText(
			{ s_Context->m_Origin.x + (s_Style.m_HeaderTextOffset.x + nodePos.x) * s_Context->m_Scale, s_Context->m_Origin.y + (s_Style.m_HeaderTextOffset.y + nodePos.y) * s_Context->m_Scale },
			IM_COL32(255, 255, 255, 255),
			node->m_Title.c_str());
		ImGui::SetWindowFontScale(1.0f);

		// Draw Node's pins here
		DrawNodePins(node);

		// Draw frame if hovered over Node
		if (IsNodeSelected(node))
		{
			s_Context->m_DrawList->AddRect(
				{ s_Context->m_Origin.x + nodePos.x * s_Context->m_Scale - s_Style.m_SelectionThinckness, s_Context->m_Origin.y + nodePos.y * s_Context->m_Scale - s_Style.m_SelectionThinckness },
				{ s_Context->m_Origin.x + (nodePos.x + node->m_Size.x) * s_Context->m_Scale + s_Style.m_SelectionThinckness, s_Context->m_Origin.y + (nodePos.y + node->m_Size.y) * s_Context->m_Scale + s_Style.m_SelectionThinckness },
				NODE_HOVER_COLOR,
				s_Style.m_NodeRounding * s_Context->m_Scale, ImDrawFlags_RoundCornersAll, s_Style.m_SelectionThinckness);
		}
	}

	Stimpi::Node* NodePanel::GetNodeByID(uint32_t id)
	{
		for (auto& node : s_Context->m_ActiveGraph->m_Nodes)
		{
			if (node->m_ID == id)
				return node.get();
		}

		return nullptr;
	}

	void NodePanel::DrawNodePins(Node* node)
	{
		ImVec2 pinStartPos = node->m_Pos;
		pinStartPos.x += s_Style.m_PinOffset + s_Style.m_PinRadius;
		pinStartPos.y += s_Style.m_HeaderHeight + s_Style.m_PinOffset + s_Style.m_PinRadius;

		for (auto pin : node->m_InPins)
		{
			DrawPin(pin.get(), pinStartPos);
			pinStartPos.y += s_Style.m_PinSpacing + s_Style.m_PinRadius; // + radius because we draw from pin center
		}

		pinStartPos = node->m_Pos;
		pinStartPos.y += s_Style.m_HeaderHeight + s_Style.m_PinOffset + s_Style.m_PinRadius;

		for (auto pin : node->m_OutPins)
		{
			DrawPin(pin.get(), pinStartPos);
			pinStartPos.y += s_Style.m_PinSpacing + s_Style.m_PinRadius; // + radius because we draw from pin center
		}
	}

	void NodePanel::DrawPin(Pin* pin, ImVec2 pos)
	{
		ImVec2 pinStartPos = pos;

		// Adjust position for OUT pin
		if (pin->m_Type == Pin::Type::OUTPUT)
		{
			ImVec2 nodeSize = pin->m_ParentNode->m_Size;
			// move draw cursor to the end of node space
			// assume correct Y pos is given already
			pinStartPos.x += nodeSize.x - s_Style.m_PinOffset - GetPinSpaceWidth() + s_Style.m_PinRadius;
		}

		// Pin hit box pos
		pin->m_Pos = pinStartPos;

		// Circle part
		float circleSegments = 10.0f * s_Context->m_Scale;
		circleSegments = (circleSegments > 5) ? ((circleSegments < 20) ? circleSegments : 20) : 5;
		if (pin->m_Connected)
		{
			s_Context->m_DrawList->AddCircleFilled(
				{ s_Context->m_Origin.x + pinStartPos.x * s_Context->m_Scale, s_Context->m_Origin.y + pinStartPos.y * s_Context->m_Scale },
				s_Style.m_PinRadius * s_Context->m_Scale,
				IM_COL32(35, 195, 35, 255),
				circleSegments);
		}
		else
		{
			s_Context->m_DrawList->AddCircle(
				{ s_Context->m_Origin.x + pinStartPos.x * s_Context->m_Scale, s_Context->m_Origin.y + pinStartPos.y * s_Context->m_Scale },
				s_Style.m_PinRadius * s_Context->m_Scale,
				IM_COL32(35, 195, 35, 255),
				circleSegments,
				s_Style.m_PinThickness * s_Context->m_Scale);
		}

		// Arrow part
		ImVec2 pinArrowP1 = pinStartPos; // first point of arrow triangle - top
		pinArrowP1.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing;
		pinArrowP1.y -= s_Style.m_PinArrowHalfHeight;
		ImVec2 pinArrowP2 = pinStartPos; // middle
		pinArrowP2.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth;
		ImVec2 pinArrowP3 = pinStartPos; // bottom
		pinArrowP3.x += s_Style.m_PinRadius + s_Style.m_PinArrowSpacing;
		pinArrowP3.y += s_Style.m_PinArrowHalfHeight;

		s_Context->m_DrawList->AddTriangleFilled(
			{ s_Context->m_Origin.x + pinArrowP1.x * s_Context->m_Scale, s_Context->m_Origin.y + pinArrowP1.y * s_Context->m_Scale },
			{ s_Context->m_Origin.x + pinArrowP2.x * s_Context->m_Scale, s_Context->m_Origin.y + pinArrowP2.y * s_Context->m_Scale },
			{ s_Context->m_Origin.x + pinArrowP3.x * s_Context->m_Scale, s_Context->m_Origin.y + pinArrowP3.y * s_Context->m_Scale },
			IM_COL32(35, 195, 35, 255));

		// Pin text part
		ImVec2 textPos = pinStartPos;
		ImVec2 textSize = ImGui::CalcTextSize(pin->m_Variable->m_Text.c_str());
		textSize.x += s_Style.m_PinTextSpacing;

		if (pin->m_Type == Pin::Type::OUTPUT)
		{
			textPos.x -= s_Style.m_PinRadius + textSize.x;
			textPos.y -= s_Style.m_PinRadius + 1.0f;
		}
		else
		{
			textPos.x += GetPinSpaceWidth();
			textPos.y -= s_Style.m_PinRadius + 1.0f; // TODO: consider changing
		}

		ImGui::SetWindowFontScale(s_Context->m_Scale);
		s_Context->m_DrawList->AddText(
			{ s_Context->m_Origin.x + textPos.x * s_Context->m_Scale, s_Context->m_Origin.y + textPos.y * s_Context->m_Scale },
			IM_COL32(255, 255, 255, 255),
			pin->m_Variable->m_Text.c_str());
		ImGui::SetWindowFontScale(1.0f);

		// Test draw Pin <-> FloatingTarget connection
		if (pin == s_Context->m_SelectedPin && s_Context->m_Action == ControllAction::NODE_PIN_DRAG)
		{
			ImVec2 startPoint = pin->m_Pos;
			ImVec2 endPoint = s_Context->m_PinFloatingTarget;
			
			DrawBezierLine(startPoint, endPoint);
		}

		// Draw pin connections
		for (auto target : pin->m_ConnectedPins)
		{
			if (target)
			{
				DrawPinToPinConnection(pin, target);
			}
		}

		// Draw debug layer
		if (s_Context->m_DebugOn)
		{
			s_Context->m_DrawList->AddRect(
				{ s_Context->m_Origin.x + (pin->m_Pos.x - s_Style.m_PinRadius) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y - s_Style.m_PinRadius) * s_Context->m_Scale },
				{ s_Context->m_Origin.x + (pin->m_Pos.x + s_Style.m_PinRadius) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y + s_Style.m_PinRadius) * s_Context->m_Scale },
				IM_COL32(225, 25, 25, 255));

			std::string dbgText = fmt::format("C: {}", pin->m_ConnectedPins.size());
			ImVec2 dbgTextPos = pinStartPos;
			dbgTextPos.x = pin->m_Type == Pin::Type::INPUT ? 
				dbgTextPos.x - GetPinSpaceWidth() - ImGui::CalcTextSize(dbgText.c_str()).x :
				dbgTextPos.x + GetPinSpaceWidth();

			ImGui::SetWindowFontScale(s_Context->m_Scale);
			s_Context->m_DrawList->AddText(
				{ s_Context->m_Origin.x + dbgTextPos.x * s_Context->m_Scale, s_Context->m_Origin.y + dbgTextPos.y * s_Context->m_Scale },
				IM_COL32(255, 25, 25, 255),
				dbgText.c_str());
			ImGui::SetWindowFontScale(1.0f);
		}
	}

	void NodePanel::DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col)
	{
		ImVec2 startPoint = start;
		ImVec2 endPoint = end;
		ImVec2 middlePoint1 = CalcFirstMidBezierPoint(startPoint, endPoint);
		ImVec2 middlePoint2 = CalcLastMidBezierPoint(startPoint, endPoint);
		s_Context->m_DrawList->AddBezierCubic(
			{ s_Context->m_Origin.x + startPoint.x * s_Context->m_Scale, s_Context->m_Origin.y + startPoint.y * s_Context->m_Scale },
			{ s_Context->m_Origin.x + middlePoint1.x * s_Context->m_Scale, s_Context->m_Origin.y + middlePoint1.y * s_Context->m_Scale },
			{ s_Context->m_Origin.x + middlePoint2.x * s_Context->m_Scale, s_Context->m_Origin.y + middlePoint2.y * s_Context->m_Scale },
			{ s_Context->m_Origin.x + endPoint.x * s_Context->m_Scale, s_Context->m_Origin.y + endPoint.y * s_Context->m_Scale },
			col,
			3.0f,
			s_Style.m_ConnectionSegments);

		if (s_Context->m_DebugOn)
		{
			s_Context->m_DrawList->AddCircleFilled(
				{ s_Context->m_Origin.x + startPoint.x * s_Context->m_Scale, s_Context->m_Origin.y + startPoint.y * s_Context->m_Scale },
				3.0f, IM_COL32(225, 25, 25, 255), 6);
			s_Context->m_DrawList->AddCircleFilled(
				{ s_Context->m_Origin.x + middlePoint1.x * s_Context->m_Scale, s_Context->m_Origin.y + middlePoint1.y * s_Context->m_Scale },
				3.0f, IM_COL32(225, 25, 25, 255), 6);
			s_Context->m_DrawList->AddCircleFilled(
				{ s_Context->m_Origin.x + middlePoint2.x * s_Context->m_Scale, s_Context->m_Origin.y + middlePoint2.y * s_Context->m_Scale },
				3.0f, IM_COL32(225, 25, 25, 255), 6);
			s_Context->m_DrawList->AddCircleFilled(
				{ s_Context->m_Origin.x + endPoint.x * s_Context->m_Scale, s_Context->m_Origin.y + endPoint.y * s_Context->m_Scale },
				3.0f, IM_COL32(225, 25, 25, 255), 6);
		}
	}

	void NodePanel::DrawPinToPinConnection(Pin* src, Pin* dest, ImU32 col)
	{
		if (src == nullptr || dest == nullptr)
			return;

		ImVec2 startPoint = src->m_Pos;
		ImVec2 endPoint = dest->m_Pos;
		
		DrawBezierLine(startPoint, endPoint, col);
	}

	void NodePanel::DrawDbgPoint(ImVec2 point)
	{
		s_Context->m_DrawList->AddCircleFilled(
			{ s_Context->m_Origin.x + point.x * s_Context->m_Scale, s_Context->m_Origin.y + point.y * s_Context->m_Scale },
			3.0f, IM_COL32(225, 25, 25, 255), 6);
	}

	void NodePanel::DbgDrawConnectionLinePoints(PinConnection* connection)
	{
		if (connection && s_Context->m_DebugOn)
		{
			for (auto point : connection->m_BezierLinePoints)
			{
				DrawDbgPoint(point);
			}
		}
	}

	/**
	 *  Mouse controls
	 */

	bool NodePanel::IsMouseHoverNode(Node* node)
	{
		ImVec2 min(s_Context->m_Origin.x + node->m_Pos.x * s_Context->m_Scale, s_Context->m_Origin.y + node->m_Pos.y * s_Context->m_Scale);
		ImVec2 max(min.x + node->m_Size.x * s_Context->m_Scale, min.y + node->m_Size.y * s_Context->m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	uint32_t NodePanel::GetMouseHoverNode()
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

	void NodePanel::UpdateMouseControls()
	{
		ImGuiIO& io = ImGui::GetIO();

		uint32_t hoverNodeID = GetMouseHoverNode();	// Reset selection later, if mouse action is finished and nothing is hovered over
		static ImVec2 dragOffset(0.0f, 0.0f);
		const bool isActive = ImGui::IsItemActive(); // refers to the invisible button / node viewport
		bool  showPopup = false;

		// This messes up with our pop up :/
		/*if (!s_Context->m_IsHovered)
		{
			ST_CORE_INFO("exit m_IsHovered");
			return;
		}*/

		switch (s_Context->m_Action)
		{
		case ControllAction::NONE:
			if (hoverNodeID != 0)
			{
				s_Context->m_Action = ControllAction::NODE_HOVER;
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
			}
			else
			{
				// Clear node selection if clicked on empty space
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					s_Context->m_SelectedNode = nullptr;

					// Check for hovered connection lines and select one if found
					// Done on mouse click to avoid checking each frame for line hovers
					s_Context->m_SelectedConnection = GetMouseHoveredConnection();
					if (s_Context->m_SelectedConnection)
					{
						s_Context->m_Action = ControllAction::CONNECTION_ONPRESS;
						break;
					}
				}

				// Nothing hovered, start canvas move action on click
				if (ImGui::IsMouseClicked(CANVAS_MOVE_BUTTON))
				{
					s_Context->m_Action = ControllAction::CANVAS_MOVE;
					break;
				}

				// Nothing hovered so we can show add new node pop up
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					s_Context->m_Action = ControllAction::SHOW_POPUP_ONRELEASE;
					break;
				}
			}

			break;
		case ControllAction::NODE_HOVER:
			if (hoverNodeID == 0)
			{
				s_Context->m_Action = ControllAction::NONE;
				break;
			}
			else
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

				// Check if we are hovering any Pins
				Node* hoveredNode = GetNodeByID(hoverNodeID);
				if (hoveredNode)
				{
					Pin* hoveredPin = GetMouseHoveredPin(hoveredNode);
					if (hoveredPin)
					{

						ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							//ST_CORE_INFO("Pin {} clicked", hoveredPin->m_ID);
							// Clear Connection hover selection
							s_Context->m_SelectedConnection = nullptr;

							s_Context->m_Action = ControllAction::NODE_PIN_DRAG;
							s_Context->m_SelectedPin = hoveredPin;
							s_Context->m_PinFloatingTarget = GetNodePanelViewClickLocation();
							break; //exit case
						}
					}
				}
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				s_Context->m_Action = ControllAction::NODE_DRAGABLE;
				s_Context->m_SelectedNode = GetNodeByID(hoverNodeID);

				// Clear Connection hover selection
				s_Context->m_SelectedConnection = nullptr;
			}
			break;
		case ControllAction::NODE_DRAGABLE:
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				// Node Dragging finished - find all connections to the moved node and update points for mouse picking
				UpdateNodeConnectionsPoints(s_Context->m_SelectedNode);

				s_Context->m_Action = ControllAction::NONE;
				break;
			}
			else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

				dragOffset.x = io.MouseDelta.x / s_Context->m_Scale;
				dragOffset.y = io.MouseDelta.y / s_Context->m_Scale;

				Node* selected = s_Context->m_SelectedNode;
				if (selected != nullptr)
				{
					selected->m_Pos.x += dragOffset.x;
					selected->m_Pos.y += dragOffset.y;
				}
			}
			else
			{
				dragOffset.x = 0.0f;
				dragOffset.y = 0.0f;
			}
			break;
		case ControllAction::NODE_PIN_DRAG:
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				// clear drag target
				s_Context->m_Action = ControllAction::NONE;

				// Handle pin drag-drop
				if (hoverNodeID != 0)
				{
					Node* hoveredNode = GetNodeByID(hoverNodeID);
					if (hoveredNode != nullptr && hoveredNode != s_Context->m_SelectedPin->m_ParentNode)
					{
						Pin* target = GetMouseHoveredPin(hoveredNode);
						if (target)
						{
							//ST_CORE_INFO("Target Node::Pin {} - {} selected", hoveredNode->m_ID, target->m_ID);
							// Handle forming Pin - Pin connection
							ConnectPinToPin(s_Context->m_SelectedPin, target);
						}
					}
				}

				s_Context->m_SelectedPin = nullptr;
				break;
			}
			else  // Mouse button still pressed - node pin dragging
			{
				// Update floating target
				s_Context->m_PinFloatingTarget = GetNodePanelViewClickLocation();
			}
			break;
		case ControllAction::CONNECTION_ONPRESS:
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				//ST_CORE_INFO("Hovering stopped - back to none");
				s_Context->m_Action = ControllAction::NONE;
				break;
			}
			else
			{
				ImVec2 mouseDragDelta = ImGui::GetMouseDragDelta();
				if (mouseDragDelta.x != 0 || mouseDragDelta.y != 0)
				{
					//ST_CORE_INFO("Dragging connection - mouse delta {}, {}", mouseDragDelta.x, mouseDragDelta.y);
					PinConnection* connection = s_Context->m_SelectedConnection;
					if (connection)
					{
						s_Context->m_Action = ControllAction::NODE_PIN_DRAG;
						s_Context->m_SelectedPin = connection->m_SourcePin;
						s_Context->m_PinFloatingTarget = GetNodePanelViewClickLocation();

						BreakPinToPinConnection(connection);
						s_Context->m_SelectedConnection = nullptr;
						break;
					}
				}
			}

			break;
		case ControllAction::SHOW_POPUP_ONRELEASE:
 			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
 			{
				showPopup = true;
				s_Context->m_Action = ControllAction::NONE;
			}
			break;
		case ControllAction::CANVAS_MOVE:
			if (s_Context->m_IsActive && ImGui::IsMouseDragging(CANVAS_MOVE_BUTTON))
			{
				s_Context->m_Scrolling.x += io.MouseDelta.x;
				s_Context->m_Scrolling.y += io.MouseDelta.y;
				// updated origin when changing scroll offset, avoids 1 frame update delay
				s_Context->m_Origin = { s_Context->m_Canvas.m_PosMin.x + s_Context->m_Scrolling.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Scrolling.y };
			}


			if (ImGui::IsMouseReleased(CANVAS_MOVE_BUTTON))
			{
				s_Context->m_Action = ControllAction::NONE;
				break;
			}
			break;
		default:
			break;
		}

		AddNodePopup(showPopup);
	}

	void NodePanel::AddNodePopup(bool show)
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
			static std::vector<std::string> nodeTypeList = { "New node" };

 			if (SearchPopup::OnImGuiRender(nodeTypeList))
 			{
				showPopup = false;
 				CreateNode(s_Context->m_NewNodePos, SearchPopup::GetSelection());
	 		}
		}
	}

	void NodePanel::HandleKeyPresses()
	{
		if (InputManager::Instance()->IsKeyPressed(ST_KEY_DELETE))
		{
			if (s_Context->m_SelectedNode)
			{
				// Remove Node
				RemoveNode(s_Context->m_SelectedNode);
				s_Context->m_SelectedNode = nullptr;
			}

			if (s_Context->m_SelectedConnection)
			{
				BreakPinToPinConnection(s_Context->m_SelectedConnection);
				s_Context->m_SelectedConnection = nullptr;
			}
		}
	}

#pragma endregion NodePanelClass

}