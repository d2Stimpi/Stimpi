#pragma once

#include "Stimpi/Scene/Assets/AssetManager.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	struct Node;
	struct PinConnection;

	struct Pin
	{
		enum class Type { INPUT = 0, OUTPUT };

		Node* m_ParentNode;	// Owner of the pin
		uint32_t m_ID;	    // Pin ID

		bool m_Connected = false;
		bool m_SingleConnection = false;
		std::vector<Pin*> m_ConnectedPins;

		std::string m_Text;
		Type m_Type;

		// For handling selection - global position
		ImVec2 m_Pos = { 0.0f, 0.0f };
	};

	struct Node
	{
		ImVec2 m_Pos;
		ImVec2 m_Size;
		uint32_t m_ID = 0;

		std::string m_Title;

		// List of Pins
		std::vector<std::shared_ptr<Pin>> m_InPins;
		std::vector<std::shared_ptr<Pin>> m_OutPins;

		Node() = default;
		Node(const Node&) = default;
		Node(const ImVec2& pos) : m_Pos(pos) {}

		static bool IsValid(Node& node) { return node.m_ID != 0; }
	};

	class NodePanel
	{
	public:
		NodePanel();
		~NodePanel();

		void SetDrawList(ImDrawList* drawList);
		void OnImGuiRender();

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