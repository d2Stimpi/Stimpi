#include "stpch.h"
#include "Gizmo2D.h"

#include "Stimpi/Log.h"
#include "Stimpi/Core/InputManager.h"
#include "Stimpi/Scene/Component.h"

#include "ImGui/src/imgui_internal.h"

#define GIZMO_DEBUG false

namespace Stimpi
{
	enum class ManipulateAxis { X_AXIS, Y_AXIS, XY_AXIS, NONE };

	struct Context
	{
		ImDrawList* m_DrawList;
		ImVec2 m_ClickHoldPos;
		bool m_MouseHold;
		bool m_Clicked; // To avoid manipulating when selecting Entity and holding mouse button
		ManipulateAxis m_UsingAxis;
		Entity m_Entity;
		float m_CameraZoom;
		GizmoAction m_Action;
		bool m_Using; // When mouse is hovered over controls, use ot prevent running mouse picking

		Context() : m_MouseHold(false), m_UsingAxis(ManipulateAxis::NONE), m_Clicked(false), m_Using(false)
		{
		}
	};

	static Context gContext;


	static bool IsAxisControlHovering(ImVec2 min, ImVec2 max)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec2 mousePos = io.MousePos;

		if ((mousePos.x >= min.x) && (mousePos.x <= max.x) &&
			(mousePos.y <= min.y) && (mousePos.y >= max.y))
		{
			return true;
		}

		return false;
	}

	static void CalcualteMove(ManipulateAxis axis)
	{
		ImGuiIO& io = ImGui::GetIO();
		
		if (InputManager::Instance()->IsMouseButtonPressed(ST_BUTTON_LEFT) && gContext.m_Clicked)
		{
			// Record initial click position
			if (gContext.m_MouseHold == false)
			{
				gContext.m_ClickHoldPos = io.MousePos;
			}
			else
			{
				// Calculate mouse move amount
				if (axis == ManipulateAxis::X_AXIS)
				{
					float translate_x = io.MousePos.x - gContext.m_ClickHoldPos.x;
					auto& quad = gContext.m_Entity.GetComponent<QuadComponent>();
					if (gContext.m_Action == GizmoAction::TRANSLATE)
					{
						quad.m_X += translate_x * gContext.m_CameraZoom;
					}
					if (gContext.m_Action == GizmoAction::SCALE)
					{
						quad.m_Width += translate_x * gContext.m_CameraZoom;
						quad.m_X -= translate_x * gContext.m_CameraZoom / 2;
					}
				}
				
				if (axis == ManipulateAxis::Y_AXIS)
				{
					float translate_y = io.MousePos.y - gContext.m_ClickHoldPos.y;
					auto& quad = gContext.m_Entity.GetComponent<QuadComponent>();
					if (gContext.m_Action == GizmoAction::TRANSLATE)
					{
						quad.m_Y -= translate_y * gContext.m_CameraZoom;
					}
					if (gContext.m_Action == GizmoAction::SCALE)
					{
						quad.m_Height -= translate_y * gContext.m_CameraZoom;
						quad.m_Y += translate_y * gContext.m_CameraZoom / 2;
					}
				}

				if (axis == ManipulateAxis::XY_AXIS)
				{
					float translate_x = io.MousePos.x - gContext.m_ClickHoldPos.x;
					float translate_y = io.MousePos.y - gContext.m_ClickHoldPos.y;
					auto& quad = gContext.m_Entity.GetComponent<QuadComponent>();
					if (gContext.m_Action == GizmoAction::TRANSLATE)
					{
						quad.m_X += translate_x * gContext.m_CameraZoom;
						quad.m_Y -= translate_y * gContext.m_CameraZoom;
					}
					if (gContext.m_Action == GizmoAction::SCALE)
					{ 
						quad.m_Width += translate_x * gContext.m_CameraZoom;
						quad.m_Height -= translate_y * gContext.m_CameraZoom;
						quad.m_X -= translate_x * gContext.m_CameraZoom / 2;
						quad.m_Y += translate_y * gContext.m_CameraZoom / 2;
					}
				}

				gContext.m_ClickHoldPos = io.MousePos;
			}
			gContext.m_MouseHold = true;
			if (axis == ManipulateAxis::X_AXIS)
				gContext.m_UsingAxis = ManipulateAxis::X_AXIS;
			if (axis == ManipulateAxis::Y_AXIS)
				gContext.m_UsingAxis = ManipulateAxis::Y_AXIS;
			if (axis == ManipulateAxis::XY_AXIS)
				gContext.m_UsingAxis = ManipulateAxis::XY_AXIS;
		}
		else
		{
			gContext.m_Clicked = false;
			gContext.m_MouseHold = false;
			gContext.m_UsingAxis = ManipulateAxis::NONE;
		}

		if (io.MouseClicked[ImGuiMouseButton_Left])
		{
			gContext.m_Clicked = true;
		}
	}

	void Gizmo2D::BeginFrame()
	{
		const ImU32 flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, 0);
		ImGui::PushStyleColor(ImGuiCol_Border, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

		ImGui::Begin("Gizmo2D", NULL, flags);
		gContext.m_DrawList = ImGui::GetWindowDrawList();
		ImGui::End();

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);
	}

	void Gizmo2D::SetDrawlist(ImDrawList* drawlist)
	{
		gContext.m_DrawList = drawlist ? drawlist : ImGui::GetWindowDrawList();
	}

	bool Gizmo2D::IsUsing()
	{
		return gContext.m_Using;
	}

	static void DrawTranslationArrow(ImVec2 pos, uint32_t length, uint32_t thickness)
	{
		ImDrawList* drawList = gContext.m_DrawList;
		if (drawList == nullptr)
		{
			return;
		}
		
		ImU32 colX = ImColor(0.1f, 0.1f, 0.9f, 1.0f);
		ImU32 colY = ImColor(0.9f, 0.1f, 0.1f, 1.0f);
		ImU32 colC = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
		ImU32 colHover = ImColor(1.0f, 0.62f, 0.07f, 1.0f);

		// Check X axis hover
		if ((IsAxisControlHovering(ImVec2(pos.x + 6/* mid circle r*/, pos.y + 6), ImVec2(pos.x + length + 15, pos.y - 6)) && gContext.m_UsingAxis == ManipulateAxis::NONE)
			|| gContext.m_UsingAxis == ManipulateAxis::X_AXIS)
		{
			colX = colHover;
			gContext.m_Using = true;
			CalcualteMove(ManipulateAxis::X_AXIS);
		}
		else
		// Check Y axis hover
		if ((IsAxisControlHovering(ImVec2(pos.x - 6, pos.y - 6/* mid circle r*/), ImVec2(pos.x + 6, pos.y - length - 15)) && gContext.m_UsingAxis == ManipulateAxis::NONE) 
			|| gContext.m_UsingAxis == ManipulateAxis::Y_AXIS)
		{
			colY = colHover;
			gContext.m_Using = true;
			CalcualteMove(ManipulateAxis::Y_AXIS);
		}
		else
		// Check Center hover
		if ((IsAxisControlHovering(ImVec2(pos.x - 6, pos.y + 6), ImVec2(pos.x + 6, pos.y - 6)) && gContext.m_UsingAxis == ManipulateAxis::NONE) 
			|| gContext.m_UsingAxis == ManipulateAxis::XY_AXIS)
		{
			colC = colHover;
			gContext.m_Using = true;
			CalcualteMove(ManipulateAxis::XY_AXIS);
		}
		else
		{
			gContext.m_Using = false;
		}

		drawList->AddLine(pos, ImVec2(pos.x + length, pos.y), colX, thickness);
		drawList->AddLine(pos, ImVec2(pos.x, pos.y - length), colY, thickness);

		// Arrow head X axis
		ImVec2 arrowPosX(pos.x + length, pos.y + 1);	// + 1 to center the arrow
		drawList->AddTriangleFilled(ImVec2(arrowPosX.x, arrowPosX.y - 6), ImVec2(arrowPosX.x, arrowPosX.y + 6), ImVec2(arrowPosX.x + 15, arrowPosX.y), colX);

		// Arrow head Y axis
		ImVec2 arrowPosY(pos.x + 1, pos.y - length);	// + 1 to center the arrow
		drawList->AddTriangleFilled(ImVec2(arrowPosY.x - 6, arrowPosY.y), ImVec2(arrowPosY.x + 6, arrowPosY.y), ImVec2(arrowPosY.x, arrowPosY.y - 15), colY);
		
		drawList->AddCircleFilled(pos, 6, colC, 10);

		// Draw Axis Control bounding Box
		if (GIZMO_DEBUG)
		{
			// X axis
			drawList->AddQuad(ImVec2(pos.x + 6, pos.y + 6), ImVec2(pos.x + length + 15, pos.y + 6),
				ImVec2(pos.x + length + 15, pos.y - 6), ImVec2(pos.x, pos.y - 6), ImColor(1.f, 0.f, 0.f, 1.f));
			// Y axis
			drawList->AddQuad(ImVec2(pos.x - 6, pos.y), ImVec2(pos.x + 6, pos.y),
				ImVec2(pos.x + 6, pos.y - length - 15), ImVec2(pos.x - 6, pos.y - length - 15), ImColor(1.f, 0.f, 0.f, 1.f));
		}
	}

	static void DrawScaleArrow(ImVec2 pos, uint32_t length, uint32_t thickness)
	{
		ImDrawList* drawList = gContext.m_DrawList;
		if (drawList == nullptr)
		{
			return;
		}

		ImU32 colX = ImColor(0.1f, 0.1f, 0.9f, 1.0f);
		ImU32 colY = ImColor(0.9f, 0.1f, 0.1f, 1.0f);
		ImU32 colC = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
		ImU32 colHover = ImColor(1.0f, 0.62f, 0.07f, 1.0f);

		// Check X axis hover
		if ((IsAxisControlHovering(ImVec2(pos.x + 6/* mid circle r*/, pos.y + 6), ImVec2(pos.x + length + 15, pos.y - 6)) && gContext.m_UsingAxis == ManipulateAxis::NONE)
			|| gContext.m_UsingAxis == ManipulateAxis::X_AXIS)
		{
			colX = colHover;
			gContext.m_Using = true;
			CalcualteMove(ManipulateAxis::X_AXIS);
		}
		else
		// Check Y axis hover
		if ((IsAxisControlHovering(ImVec2(pos.x - 6, pos.y - 6/* mid circle r*/), ImVec2(pos.x + 6, pos.y - length - 15)) && gContext.m_UsingAxis == ManipulateAxis::NONE)
			|| gContext.m_UsingAxis == ManipulateAxis::Y_AXIS)
		{
			colY = colHover;
			gContext.m_Using = true;
			CalcualteMove(ManipulateAxis::Y_AXIS);
		}
		else
		// Check Center hover
		if ((IsAxisControlHovering(ImVec2(pos.x - 6, pos.y + 6), ImVec2(pos.x + 6, pos.y - 6)) && gContext.m_UsingAxis == ManipulateAxis::NONE)
			|| gContext.m_UsingAxis == ManipulateAxis::XY_AXIS)
		{
			colC = colHover;
			gContext.m_Using = true;
			CalcualteMove(ManipulateAxis::XY_AXIS);
		}
		else
		{
			gContext.m_Using = false;
		}

		drawList->AddLine(pos, ImVec2(pos.x + length, pos.y), colX, thickness);
		drawList->AddLine(pos, ImVec2(pos.x, pos.y - length), colY, thickness);

		// Arrow head X axis
		ImVec2 arrowPosX(pos.x + length, pos.y);	// + 1 to center the arrow
		drawList->AddQuadFilled(ImVec2(arrowPosX.x, arrowPosX.y + 5), ImVec2(arrowPosX.x + 10, arrowPosX.y + 5), ImVec2(arrowPosX.x + 10, arrowPosX.y - 5), ImVec2(arrowPosX.x, arrowPosX.y - 5), colX);

		// Arrow head Y axis
		ImVec2 arrowPosY(pos.x + 1, pos.y - length);	// + 1 to center the arrow
		drawList->AddQuadFilled(ImVec2(arrowPosY.x + 5, arrowPosY.y), ImVec2(arrowPosY.x + 5, arrowPosY.y - 10), ImVec2(arrowPosY.x - 5, arrowPosY.y - 10), ImVec2(arrowPosY.x - 5, arrowPosY.y), colY);

		drawList->AddCircleFilled(pos, 6, colC, 10);

		// Draw Axis Control bounding Box
		if (GIZMO_DEBUG)
		{
			// X axis
			drawList->AddQuad(ImVec2(pos.x + 6, pos.y + 6), ImVec2(pos.x + length + 15, pos.y + 6),
				ImVec2(pos.x + length + 15, pos.y - 6), ImVec2(pos.x, pos.y - 6), ImColor(1.f, 0.f, 0.f, 1.f));
			// Y axis
			drawList->AddQuad(ImVec2(pos.x - 6, pos.y), ImVec2(pos.x + 6, pos.y),
				ImVec2(pos.x + 6, pos.y - length - 15), ImVec2(pos.x - 6, pos.y - length - 15), ImColor(1.f, 0.f, 0.f, 1.f));
		}
	}

	void Gizmo2D::Manipulate(Camera* camera, Entity object, GizmoAction action)
	{
		auto quad = object.GetComponent<QuadComponent>();
		glm::vec2 objPos = { quad.m_X + quad.m_Width / 2.0f, quad.m_Y + quad.m_Height / 2.0f };
		glm::vec3 camPos = camera->GetPosition();
		float camZoom = camera->GetZoomFactor();

		glm::vec2 drawPos = (objPos - glm::vec2{ camPos.x, camPos.y }) / camZoom;

		ImVec2 winPos = ImGui::GetCursorScreenPos();
		ImVec2 winSize = ImGui::GetContentRegionAvail();
		
		gContext.m_Action = action;
		gContext.m_Entity = object;
		gContext.m_CameraZoom = camera->GetZoomFactor();

		if (action == GizmoAction::TRANSLATE)
			DrawTranslationArrow(ImVec2(drawPos.x + winPos.x, winPos.y + winSize.y - drawPos.y), 40, 4);

		if (action == GizmoAction::SCALE)
			DrawScaleArrow(ImVec2(drawPos.x + winPos.x, winPos.y + winSize.y - drawPos.y), 40, 4);
	}
}