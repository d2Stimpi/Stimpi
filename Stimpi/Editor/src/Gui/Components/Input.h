#pragma once

#include <glm/glm.hpp>

#include "ImGui/src/imgui.h"

namespace Stimpi
{
	namespace UI
	{
		enum class UndoCmdType { VALUE, ENTITY };

		class Input
		{
		public:
			static void SetUndoCmdContext(UndoCmdType type);

			static bool DragFloat3(const char* label, glm::vec3& val, bool* done = nullptr, bool saveCmd = true);
			static bool DragFloat2(const char* label, glm::vec2& val, bool* done = nullptr, bool saveCmd = true);
			static bool DragFloat(const char* label, float& val, float speed = 1.0f, float min = 0.0f, float max = 0.0f, bool* done = nullptr, bool saveCmd = true);

			static glm::vec3 GetStartFloat3();
			static glm::vec2 GetStartFloat2();
			static float GetStartFloat();

			static bool InputText(const char* label, char* buf, size_t size, ImGuiInputTextFlags flags = 0);
			static bool InputInt(const char* label, int* val, int step = 1, int set_fast = 100);
			static bool InputFloat(const char* label, float* val, float step = 0.0f, float step_fast = 0.0f);
			static bool ColorEdit4(const char* label, glm::vec4& val, bool* done = nullptr, bool saveCmd = true);

			static bool ButtonFileInput(const char* label, const char* fileName);
			static bool Checkbox(const char* label, bool* val);

			static bool BeginCombo(const char* label, const char* preview);
			static void EndCombo();
			static bool Selectable(const char* label, bool selected);
		};
	}
}