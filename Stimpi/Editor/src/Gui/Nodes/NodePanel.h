#pragma once

#include "Stimpi/Scene/Assets/AssetManager.h"

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	struct Node
	{
		ImVec2 m_Pos;
		ImVec2 m_Size;
		uint32_t m_ID = 0;

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
		void CreateNode(ImVec2 pos);
		void DrawNode(Node& node);

		// Node helpers
		Node* GetNodeByID(uint32_t id);

		// Mouse control methods
		bool IsMouseHoverNode(Node& node);
		uint32_t GetMouseHoverNode();
		void UpdateMouseControls();


	private:
		AssetHandle m_HeaderImage;
	};
}