#include "stpch.h"
#include "Gui/Components/Input.h"

#include "Stimpi/Log.h"
#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

#include <glm/gtc/type_ptr.hpp>

namespace Stimpi
{
	namespace UI
	{
		using input_variant = std::variant<glm::vec3, glm::vec2, float, int>;

		struct InputContext
		{
			bool m_IsInputActive = false;
			input_variant m_Temp;
		};

		InputContext s_Context;

		bool Input::DragFloat3(const char* label, glm::vec3& vec)
		{
			bool retVal = false;

			retVal = ImGui::DragFloat3(label, glm::value_ptr(vec));

			if (ImGui::IsItemActive() && !s_Context.m_IsInputActive)
			{
				s_Context.m_IsInputActive = true;
				s_Context.m_Temp = vec;
			}
			if (!ImGui::IsItemActive() && s_Context.m_IsInputActive)
			{
				s_Context.m_IsInputActive = false;
				if (std::get<glm::vec3>(s_Context.m_Temp) != vec)
				{
					ST_CORE_INFO("Input finished, new value: {}", vec);
					// Create cmd here
				}
			}

			return retVal;
		}

	}
}
