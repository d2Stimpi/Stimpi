#include "stpch.h"
#include "Gui/Nodes/NodePanel.h"

#include "Stimpi/Log.h"

#include "ImGui/src/imgui_internal.h"

#define NODE_HOVER_COLOR	IM_COL32(220, 220, 30, 255)

#define CANVAS_MOVE_BUTTON ImGuiMouseButton_Left

namespace Stimpi
{
	bool s_Show = false;	// Easier use in menu if static - to toggle window visibility

	enum class ControllAction { NONE = 0, NODE_HOVER, NODE_DRAGABLE, SHOW_POPUP_ONRELEASE, CANVAS_MOVE, NODE_PIN_DRAG };

	struct NodePanelStyle
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

		// Grid
		float m_GridStep;
	};

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
		std::vector<std::shared_ptr<Node>> m_Nodes;

		ImVec2 m_Origin = { 0.0f, 0.0f };	// Global panel view translate offset
		ImVec2 m_Scrolling = { 0.0f, 0.0f };
		NodeCanvas m_Canvas;
		float m_Scale = 1.0f;

		bool m_IsHovered = false;
		bool m_IsActive = false;

		// Convenience to avoid iterating Nodes vector
		Node* m_SelectedNode = nullptr;
		Pin* m_SelectedPin = nullptr;

		ImDrawList* m_DrawList;

		// Controls
		ControllAction m_Action = ControllAction::NONE;

		// Debug
		bool m_DebugOn = false;
	};

	static NodePanelContext* s_Context;
	static NodePanelStyle s_Style;

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

		s_Style.m_GridStep = 16.0f;
	}

	/**
	 * Helper Node static functions
	 */

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

	/**
	 * Helper Pin static functions
	 */

	float GetPinSpaceHeight()
	{
		return s_Style.m_PinRadius * 2.0f;
	}

	float GetPinSpaceWidth()
	{
		return s_Style.m_PinRadius * 2.0f + s_Style.m_PinArrowSpacing + s_Style.m_PinArrowWidth;
	}

	bool IsMouseHovePin(Pin* pin)
	{
		ImVec2 min(s_Context->m_Origin.x + (pin->m_Pos.x - s_Style.m_PinRadius - s_Style.m_PinSelectOffset) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y - s_Style.m_PinRadius - s_Style.m_PinSelectOffset) * s_Context->m_Scale);
		ImVec2 max(s_Context->m_Origin.x + (pin->m_Pos.x + s_Style.m_PinRadius + s_Style.m_PinSelectOffset) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y + s_Style.m_PinRadius + s_Style.m_PinSelectOffset) * s_Context->m_Scale);

		return ImGui::IsMouseHoveringRect(min, max, true /*clip*/);
	}

	/**
	 *  NodePanel Class
	 */

	NodePanel::NodePanel()
	{
		m_HeaderImage = AssetManager::GetAsset<Texture>("..\/assets\/textures\/Gradient2.png");
		s_Context = new NodePanelContext();

		InitNodePanelStyle();

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

			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			s_Context->m_DrawList = draw_list;

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
			for (auto node : s_Context->m_Nodes)
			{
				DrawNode(node.get());
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
		newNode->m_ID = ++s_Context->m_NextNodeID;
		newNode->m_Title = title;

		// Add some pins for testing
		std::shared_ptr<Pin> pin = std::make_shared<Pin>();
		pin->m_ID = 1;
		pin->m_ParentNode = newNode.get();
		pin->m_Text = "Something";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin);

		std::shared_ptr<Pin> pin2 = std::make_shared<Pin>();
		pin2->m_ID = 2;
		pin2->m_ParentNode = newNode.get();
		pin2->m_Text = "Else";
		pin->m_Type = Pin::Type::INPUT;
		newNode->m_InPins.emplace_back(pin2);

		std::shared_ptr<Pin> pin3 = std::make_shared<Pin>();
		pin3->m_ID = 3;
		pin3->m_ParentNode = newNode.get();
		pin3->m_Text = "Output";
		pin3->m_Type = Pin::Type::OUTPUT;
		newNode->m_OutPins.emplace_back(pin3);

		s_Context->m_Nodes.emplace_back(newNode);
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
		for (auto& node : s_Context->m_Nodes)
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
		ImVec2 textSize = ImGui::CalcTextSize(pin->m_Text.c_str());
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
			pin->m_Text.c_str());
		ImGui::SetWindowFontScale(1.0f);

		// Draw debug layer
		if (s_Context->m_DebugOn)
		{
			s_Context->m_DrawList->AddRect(
				{ s_Context->m_Origin.x + (pin->m_Pos.x - s_Style.m_PinRadius) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y - s_Style.m_PinRadius) * s_Context->m_Scale },
				{ s_Context->m_Origin.x + (pin->m_Pos.x + s_Style.m_PinRadius) * s_Context->m_Scale, s_Context->m_Origin.y + (pin->m_Pos.y + s_Style.m_PinRadius) * s_Context->m_Scale },
				IM_COL32(225, 25, 25, 255));
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
		for (auto node : s_Context->m_Nodes)
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

// 		if (hoverNodeID != 0)
// 			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);

		switch (s_Context->m_Action)
		{
		case ControllAction::NONE:
			if (hoverNodeID != 0)
			{
				s_Context->m_Action = ControllAction::NODE_HOVER;
			}
			else
			{
				// Clear node selection if clicked on empty space
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					s_Context->m_SelectedNode = nullptr;
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
				// Check if we are hovering any Pins
				bool clickedPin = false;

				Node* hoveredNode = GetNodeByID(hoverNodeID);
				for (auto pin : hoveredNode->m_InPins)
				{
					if (IsMouseHovePin(pin.get()))
					{
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							ST_CORE_INFO("Pin {} clicked", pin->m_ID);
							pin->m_Connected = !pin->m_Connected;

							s_Context->m_Action = ControllAction::NODE_PIN_DRAG;
							s_Context->m_SelectedPin = pin.get();
							clickedPin = true;	// exit case condition -> NODE_PIN_DRAG state
							break;
						}
					}
				}

				if (clickedPin)
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
		case ControllAction::NODE_PIN_DRAG:
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				s_Context->m_Action = ControllAction::NONE;

				// TODO: handle pin drag-drop here

				s_Context->m_SelectedPin = nullptr;
				break;
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
		if (show)
			ImGui::OpenPopup("AddNodePopup");

		if (ImGui::BeginPopup("AddNodePopup"))
		{
			if (ImGui::Selectable("New Node"))
			{
				ImGuiIO& io = ImGui::GetIO();
		
				ImVec2 newPos = { (io.MousePos.x - s_Context->m_Origin.x) / s_Context->m_Scale, (io.MousePos.y  - s_Context->m_Origin.y) / s_Context->m_Scale };
				CreateNode(newPos, "New Node");
			}

			ImGui::EndPopup();
		}
	}

}