#include "stpch.h"
#include "Gui/Components/Input.h"

#include "Gui/Panels/SceneHierarchyWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Cmd/CommandStack.h"

#include "ImGui/src/imgui.h"
#include "ImGui/src/imgui_internal.h"

#include <glm/gtc/type_ptr.hpp>

namespace Stimpi
{
	namespace UI
	{

		struct InputContext
		{
			ImGuiID m_ActiveID = 0;

			glm::vec3 m_TempVec3;
			glm::vec2 m_TempVec2;
			float m_TempFloat;

			UndoCmdType m_UndoCmdType = UndoCmdType::ENTITY;
		};

		static InputContext s_Context;

		void Input::SetUndoCmdContext(UndoCmdType type)
		{
			s_Context.m_UndoCmdType = type;
		}

		bool Input::DragFloat3(const char* label, glm::vec3& val)
		{
			bool retVal = false;

			retVal = ImGui::DragFloat3(label, glm::value_ptr(val));

			if (ImGui::IsItemActive() && s_Context.m_ActiveID == 0)
			{
				s_Context.m_ActiveID = ImGui::GetActiveID();
				s_Context.m_TempVec3 = val;
			}
			if (!ImGui::IsItemActive() && s_Context.m_ActiveID != 0 && s_Context.m_ActiveID == ImGui::GetItemID())
			{
				s_Context.m_ActiveID = 0;
				glm::vec3 prevValue = s_Context.m_TempVec3;
				if (prevValue != val)
				{
					//ST_CORE_INFO("Input finished, previous value: {}", prevValue);
					// Create cmd here
					if (s_Context.m_UndoCmdType == UndoCmdType::ENTITY)
					{
						Entity entity = SceneHierarchyWindow::GetSelectedEntity();
						Command* cmd = EntityCommand::Create(entity, val - prevValue, glm::value_ptr(val));
						CommandStack::Push(cmd);
					}
					else if(s_Context.m_UndoCmdType == UndoCmdType::VALUE)
					{
						// TODO: consider if needed
					}
				}
			}

			return retVal;
		}

		bool Input::DragFloat2(const char* label, glm::vec2& val)
		{
			bool retVal = false;

			retVal = ImGui::DragFloat2(label, glm::value_ptr(val));

			if (ImGui::IsItemActive() && s_Context.m_ActiveID == 0)
			{
				s_Context.m_ActiveID = ImGui::GetActiveID();
				s_Context.m_TempVec2 = val;
			}
			if (!ImGui::IsItemActive() && s_Context.m_ActiveID != 0 && s_Context.m_ActiveID == ImGui::GetItemID())
			{
				s_Context.m_ActiveID = 0;
				glm::vec2 prevValue = s_Context.m_TempVec2;
				if (prevValue != val)
				{
					//ST_CORE_INFO("Input finished, previous value: {}", prevValue);
					// Create cmd here
					if (s_Context.m_UndoCmdType == UndoCmdType::ENTITY)
					{
						Entity entity = SceneHierarchyWindow::GetSelectedEntity();
						Command* cmd = EntityCommand::Create(entity, val - prevValue, glm::value_ptr(val));
						CommandStack::Push(cmd);
					}
					else if (s_Context.m_UndoCmdType == UndoCmdType::VALUE)
					{
						// TODO: consider if needed
					}
				}
			}

			return retVal;
		}

		bool Input::DragFloat(const char* label, float& val, float speed, float min, float max)
		{
			bool retVal = false;

			retVal = ImGui::DragFloat(label, &val, speed, min, max);

			if (ImGui::IsItemActive() && s_Context.m_ActiveID == 0)
			{
				s_Context.m_ActiveID = ImGui::GetActiveID();
				s_Context.m_TempFloat = val;
			}
			if (!ImGui::IsItemActive() && s_Context.m_ActiveID != 0 && s_Context.m_ActiveID == ImGui::GetItemID())
			{
				s_Context.m_ActiveID = 0;
				float prevValue = s_Context.m_TempFloat;
				if (prevValue != val)
				{
					ST_CORE_INFO("Input finished, previous value: {}", prevValue);
					// Create cmd here
					if (s_Context.m_UndoCmdType == UndoCmdType::ENTITY)
					{
						Entity entity = SceneHierarchyWindow::GetSelectedEntity();
						Command* cmd = EntityCommand::Create(entity, val - prevValue, &val);
						CommandStack::Push(cmd);
					}
					else if (s_Context.m_UndoCmdType == UndoCmdType::VALUE)
					{
						// TODO: consider if needed
					}
				}
			}

			return retVal;
		}

	}
}
