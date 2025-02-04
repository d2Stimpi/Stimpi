#pragma once

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	// Forwards to avoid include issues
	struct NGraphPanelCanvas;
	class NGraphPanel;
	class NNode;
	class NPin;

	class NGraphRenderer
	{
	public:
		static void SetPanelContext(NGraphPanel* panelContext);
		static void SetDrawList(ImDrawList* drawList);
		static void SetDrawCanvas(NGraphPanelCanvas* canvas);

		static ImDrawList* GetDrawList();
		static NGraphPanelCanvas* GetDrawCanvas();

		static void OnImGuiRender();

		// Debug methods
		static void SetDebugMode(bool debug);
		static bool IsDebugModeOn();

		// Node render API
		static void DrawNode(NNode* node);	
		static void DrawNodePins(NNode* node);
		static void DrawPin(NPin* pin, ImVec2 pos);
		static void DrawBezierLine(ImVec2 start, ImVec2 end, ImU32 col = IM_COL32(255, 255, 255, 255));
		static void DrawPinToPinConnection(NPin* src, NPin* dest, ImU32 col = IM_COL32(255, 255, 255, 255));

	private:
		static ImU32 GetPinVariantColor(NPin* pin);

		static ImVec2 CalcFirstMidBezierPoint(const ImVec2& start, const ImVec2& end);
		static ImVec2 CalcLastMidBezierPoint(const ImVec2& start, const ImVec2& end);
	};
}