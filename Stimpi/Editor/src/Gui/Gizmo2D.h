#pragma once

#include "ImGui/src/imgui.h"

#include "Stimpi/Scene/Camera.h"
#include "Stimpi/Scene/Entity.h"

#include <glm/glm.hpp>


/* TODO:
* - Add snapping
* - Add rotation
* - Use lerp to smooth the obj manipulation
* 
*/

namespace Stimpi
{
	enum class GizmoAction { TRANSLATE = 0, SCALE, ROTATE, SCALE_WRAP, NONE };

	class Gizmo2D
	{
	public:
		// To be called at start of the frame rendering ImGui_XXXX_NewFrame
		static void BeginFrame();
		static void SetDrawlist(ImDrawList* drawlist);

		static bool IsUsing();

		static void Manipulate(Camera* camera, Entity object, GizmoAction action);
	private:
	};
}