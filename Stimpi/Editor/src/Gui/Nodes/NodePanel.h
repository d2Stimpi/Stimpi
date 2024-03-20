#pragma once

#include "Stimpi/Scene/Assets/AssetManager.h"

#include "Gui/Nodes/GraphComponents.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	class NodePanel
	{
	public:
		NodePanel();
		~NodePanel();

		void SetDrawList(ImDrawList* drawList);
		void OnImGuiRender();

		// Graph methods
		void AddGraph(Graph* graph);
		void RemoveGraph(Graph* graph);

		static void ShowWindow(bool show);
		static bool IsVisible();
	private:
		// Grid
		void DrawCanvasGrid();

		// Node methods
		void CreateNode(ImVec2 pos, std::string title);
		void RemoveNode(Node* node);
		void DrawNode(Node* node);

		// Node helpers
		Node* GetNodeByID(uint32_t id);

		// Pin methods
		void DrawNodePins(Node* node);
		void DrawPin(Pin* pin, ImVec2 pos);
		void DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col = IM_COL32(255, 255, 255, 255));
		void DrawPinToPinConnection(Pin* src, Pin* dest, ImU32 col = IM_COL32(255, 255, 255, 255));

		// Connection methods
		void DrawDbgPoint(ImVec2 point);
		void DbgDrawConnectionLinePoints(PinConnection* connection);

		// Mouse control methods
		bool IsMouseHoverNode(Node* node);
		uint32_t GetMouseHoverNode();
		void UpdateMouseControls();
		void AddNodePopup(bool show);

		// Keyboard
		void HandleKeyPresses();

	private:
		AssetHandle m_HeaderImage;
	};
}