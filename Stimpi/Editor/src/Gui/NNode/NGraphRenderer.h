#pragma once

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	// Forwards to avoid include issues
	struct NGraphPanelCanvas;
	class NGraphPanel;
	class NNode;

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
		static void DrawNode(std::shared_ptr<NNode> node);		
	};
}