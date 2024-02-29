#include "stpch.h"
#include "Gui/Nodes/NodePanel.h"

#include "Stimpi/Log.h"

#include "ImGui/src/imgui_internal.h"

#define NODE_HOVER_COLOR	IM_COL32(220, 220, 30, 255)

namespace Stimpi
{
	bool s_Show = false;	// Easier use in menu if static - to toggle window visibility

	enum class ControllAction { NONE = 0, NODE_HOVER, NODE_DRAGABLE };

	struct NodeCanvas
	{
		ImVec2 m_PosMin = ImVec2(0.0f, 0.0f);
		ImVec2 m_PosMax = ImVec2(0.0f, 0.0f);
		ImVec2 m_Size = ImVec2(0.0f, 0.0f);

		NodeCanvas() = default;
	};

	struct NodePanelContext
	{
		uint32_t m_NextNodeID = 0;
		std::vector<Node> m_Nodes;

		ImVec2 m_Origin;	// Global panel view translate offset
		ImVec2 m_Scrolling;
		NodeCanvas m_Canvas;
		float m_Scale = 1.0f;

		// convenience to avoid iterating Nodes vector
		Node* m_SelectedNode = nullptr;

		ImDrawList* m_DrawList;

		// Controls
		ControllAction m_Action = ControllAction::NONE;
	};

	static NodePanelContext* s_Context;

	/**
	 * Helper static functions
	 */

	static bool IsNodeSelected(Node* node)
	{
		if (node != nullptr && s_Context->m_SelectedNode != nullptr)
			return node->m_ID == s_Context->m_SelectedNode->m_ID;

		return false;
	}

	/**
	 *  NodePanel
	 */

	NodePanel::NodePanel()
	{
		m_HeaderImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/Gradient2.png");
		s_Context = new NodePanelContext();

		CreateNode(ImVec2(100.0f, 250.0f));
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
			s_Context->m_Scale = scale;

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			s_Context->m_DrawList = draw_list;

			s_Context->m_Canvas.m_PosMin = ImGui::GetCursorScreenPos();		// ImDrawList API uses screen coordinates!
			s_Context->m_Canvas.m_Size = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
			if (s_Context->m_Canvas.m_Size.x < 50.0f) s_Context->m_Canvas.m_Size.x = 50.0f;
			if (s_Context->m_Canvas.m_Size.y < 50.0f) s_Context->m_Canvas.m_Size.y = 50.0f;
			s_Context->m_Canvas.m_PosMax = ImVec2(s_Context->m_Canvas.m_PosMin.x + s_Context->m_Canvas.m_Size.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Canvas.m_Size.y);

			ImGuiIO& io = ImGui::GetIO();
			ImGui::InvisibleButton("canvas", s_Context->m_Canvas.m_Size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool is_hovered = ImGui::IsItemHovered(); // Hovered
			const bool is_active = ImGui::IsItemActive();   // Held

			if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right))
			{
				s_Context->m_Scrolling.x += io.MouseDelta.x;
				s_Context->m_Scrolling.y += io.MouseDelta.y;
			}

			ImVec2 origin(s_Context->m_Canvas.m_PosMin.x + s_Context->m_Scrolling.x, s_Context->m_Canvas.m_PosMin.y + s_Context->m_Scrolling.y); // scrolled origin
			s_Context->m_Origin = origin;

			// Process mouse control before any drawing calls
			UpdateMouseControls();

			DrawCanvasGrid();

			// Draw test node frame
			/*draw_list->AddRectFilled({ origin.x + 20.0f * scale, origin.y + 20.0f * scale }, { origin.x + 300.0f * scale, origin.y + 150.0f * scale }, IM_COL32(120, 120, 120, 255), 15.0f * scale);
			
			ImVec2 uv_min = ImVec2(0.0f, 1.0f);
			ImVec2 uv_max = ImVec2(1.0f, 0.0f);
			auto texture = AssetManager::GetAsset(m_HeaderImage).As<Texture>();
			if (texture->Loaded())
			{
				draw_list->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
					{ origin.x + 20.0f * scale, origin.y + 20.0f * scale }, { origin.x + 300.0f * scale, origin.y + 50.0f * scale },
					uv_min, uv_max, IM_COL32(225, 30, 30, 255), 15 * scale, ImDrawFlags_RoundCornersTop);
			}

			ImGui::SetWindowFontScale(scale);
			draw_list->AddText({ origin.x + 45.0f * scale, origin.y + 20.0f * scale }, IM_COL32(255, 255, 255, 255), "Node name");
			ImGui::SetWindowFontScale(1.0f);*/

			// Draw all nodes
			for (auto& node : s_Context->m_Nodes)
			{
				DrawNode(node);
			}

			// Rect pushed in drawing of the Grid (start of the draw commands)
			s_Context->m_DrawList->PopClipRect();


			ImGui::End();
		}
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

		// Draw border and background color
		s_Context->m_DrawList->AddRectFilled(canvasPosMin, canvasPosMax, IM_COL32(50, 50, 50, 255));
		s_Context->m_DrawList->AddRect(canvasPosMin, canvasPosMax, IM_COL32(255, 255, 255, 255));

		// Draw grid + all lines in the canvas
		s_Context->m_DrawList->PushClipRect(canvasPosMin, canvasPosMax, true);
		{
			const float GRID_STEP = 64.0f * s_Context->m_Scale;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvasSize.x; x += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x + x, canvasPosMin.y), ImVec2(canvasPosMin.x + x, canvasPosMax.y), IM_COL32(200, 200, 200, 40));
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvasSize.y; y += GRID_STEP)
				s_Context->m_DrawList->AddLine(ImVec2(canvasPosMin.x, canvasPosMin.y + y), ImVec2(canvasPosMax.x, canvasPosMin.y + y), IM_COL32(200, 200, 200, 40));
		}
	}

	/**
	 *  Node methods
	 */

	void NodePanel::CreateNode(ImVec2 pos)
	{
		Node newNode;
		newNode.m_Pos = pos;
		newNode.m_Size = { 250.0f, 100.0f };
		newNode.m_ID = ++s_Context->m_NextNodeID;

		s_Context->m_Nodes.push_back(newNode);
	}

	void NodePanel::DrawNode(Node& node)
	{
		ImVec2 nodePos = node.m_Pos;

		s_Context->m_DrawList->AddRectFilled({ s_Context->m_Origin.x + nodePos.x * s_Context->m_Scale, s_Context->m_Origin.y + nodePos.y * s_Context->m_Scale }, { s_Context->m_Origin.x + (nodePos.x + node.m_Size.x) * s_Context->m_Scale, s_Context->m_Origin.y + (nodePos.y + node.m_Size.y) * s_Context->m_Scale }, IM_COL32(120, 120, 120, 255), 15.0f * s_Context->m_Scale);

		ImVec2 uv_min = ImVec2(0.0f, 1.0f);
		ImVec2 uv_max = ImVec2(1.0f, 0.0f);
		auto texture = AssetManager::GetAsset(m_HeaderImage).As<Texture>();
		if (texture->Loaded())
		{
			s_Context->m_DrawList->AddImageRounded((void*)(intptr_t)texture->GetTextureID(),
				{ s_Context->m_Origin.x + nodePos.x * s_Context->m_Scale, s_Context->m_Origin.y + nodePos.y * s_Context->m_Scale }, { s_Context->m_Origin.x + (nodePos.x + node.m_Size.x) * s_Context->m_Scale, s_Context->m_Origin.y + (nodePos.y + 30.0f) * s_Context->m_Scale },
				uv_min, uv_max, IM_COL32(225, 30, 30, 255), 15 * s_Context->m_Scale, ImDrawFlags_RoundCornersTop);
		}

		ImGui::SetWindowFontScale(s_Context->m_Scale);
		s_Context->m_DrawList->AddText({ s_Context->m_Origin.x + (35.0f + nodePos.x) * s_Context->m_Scale, s_Context->m_Origin.y + (8.0f + nodePos.y) * s_Context->m_Scale }, IM_COL32(255, 255, 255, 255), "Node name");
		ImGui::SetWindowFontScale(1.0f);

		// Draw frame if hovered over Node
		if (IsNodeSelected(&node)/*IsMouseHoverNode(node)*/)
		{
			s_Context->m_DrawList->AddRect(
				{ s_Context->m_Origin.x + nodePos.x * s_Context->m_Scale - 3.0f, s_Context->m_Origin.y + nodePos.y * s_Context->m_Scale - 3.0f },
				{ s_Context->m_Origin.x + (nodePos.x + node.m_Size.x) * s_Context->m_Scale + 3.0f, s_Context->m_Origin.y + (nodePos.y + node.m_Size.y) * s_Context->m_Scale + 3.0f },
				NODE_HOVER_COLOR,
				15.0f * s_Context->m_Scale, ImDrawFlags_RoundCornersAll, 3.0f);
		}
	}

	Stimpi::Node* NodePanel::GetNodeByID(uint32_t id)
	{
		for (auto& node : s_Context->m_Nodes)
		{
			if (node.m_ID == id)
				return &node;
		}

		return nullptr;
	}

	/**
	 *  Mouse controls
	 */

	bool NodePanel::IsMouseHoverNode(Node& node)
	{
		ImVec2 min(s_Context->m_Origin.x + node.m_Pos.x * s_Context->m_Scale, s_Context->m_Origin.y + node.m_Pos.y * s_Context->m_Scale);
		ImVec2 max(min.x + node.m_Size.x * s_Context->m_Scale, min.y + node.m_Size.y * s_Context->m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	uint32_t NodePanel::GetMouseHoverNode()
	{
		for (auto& node : s_Context->m_Nodes)
		{
			if (IsMouseHoverNode(node))
			{
				return node.m_ID;
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
		
// 		if (hoverNodeID != 0)
// 			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

		switch (s_Context->m_Action)
		{
		case ControllAction::NONE:
			if (hoverNodeID != 0)
			{
				s_Context->m_Action = ControllAction::NODE_HOVER;
			}

			// Clear node selection if clicked on empty space
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				s_Context->m_SelectedNode = nullptr;
			}

			break;
		case ControllAction::NODE_HOVER:
			if (hoverNodeID == 0)
			{
				s_Context->m_Action = ControllAction::NONE;
				break;
			}

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				s_Context->m_Action = ControllAction::NODE_DRAGABLE;
				s_Context->m_SelectedNode = GetNodeByID(hoverNodeID);
			}
			break;
		case ControllAction::NODE_DRAGABLE:
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				s_Context->m_Action = ControllAction::NONE;
				break;
			}
			else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
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
		default:
			break;
		}
	}
}