#include "stpch.h"
#include "Gui/Components/Input.h"

#include "Gui/Panels/SceneHierarchyWindow.h"

#include "Stimpi/Log.h"
#include "Stimpi/Cmd/CommandStack.h"

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

			// Style variables
			float m_LabelOffset = 150.0f;
			float m_InputWidth = 50.0f;
			const char* m_InputFormat = "%.2f";
		};

		static InputContext s_Context;

		/**
		 * Local static methods
		 */

		static void SetCurrentLineTextOffset()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return;

			window->DC.CurrLineTextBaseOffset = 3.0f;
		}

		/**
		 * Input methods
		 */

		void Input::SetUndoCmdContext(UndoCmdType type)
		{
			s_Context.m_UndoCmdType = type;
		}

		bool Input::DragFloat3(const char* label, glm::vec3& val, bool* done, bool saveCmd)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			bool ret = false;
			if (done) *done = false;

			const char* label_end = ImGui::FindRenderedTextEnd(label);
			SetCurrentLineTextOffset();
			ImGui::TextEx(label, label_end); ImGui::SameLine(s_Context.m_LabelOffset);

			ImGui::SetNextItemWidth(s_Context.m_InputWidth * 3 + style.ItemInnerSpacing.x * 2);
			label_end = strlen(label_end) == 0 ? label : label_end;
			ret = ImGui::DragFloat3(label_end, glm::value_ptr(val), 1.0f, 0.0f, 0.0f, s_Context.m_InputFormat);

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
					// Set to true only when value changed and input has finished 
					if (done)
					{
						*done = true;
						ret = true;
					}

					// Create cmd here
					if (saveCmd)
					{
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
			}

			return ret;
		}

		bool Input::DragFloat2(const char* label, glm::vec2& val, bool* done, bool saveCmd)
		{
			ImGuiStyle& style = ImGui::GetStyle();
			bool ret = false;
			if (done) *done = false;

			const char* label_end = ImGui::FindRenderedTextEnd(label);
			SetCurrentLineTextOffset();
			ImGui::TextEx(label, label_end); ImGui::SameLine(s_Context.m_LabelOffset);

			ImGui::SetNextItemWidth(s_Context.m_InputWidth * 2 + style.ItemInnerSpacing.x);
			label_end = strlen(label_end) == 0 ? label : label_end;
			ret = ImGui::DragFloat2(label_end, glm::value_ptr(val), 1.0f, 0.0f, 0.0f, s_Context.m_InputFormat);

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
					// Set to true only when value changed and input has finished 
					if (done)
					{
						*done = true;
						ret = true;
					}
					
					// Create cmd here
					if (saveCmd)
					{
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
			}

			return ret;
		}

		bool Input::DragFloat(const char* label, float& val, float speed, float min, float max, bool* done, bool saveCmd)
		{
			bool ret = false;
			if (done) *done = false;

			const char* label_end = ImGui::FindRenderedTextEnd(label);
			SetCurrentLineTextOffset();
			ImGui::TextEx(label, label_end); ImGui::SameLine(s_Context.m_LabelOffset);

			ImGui::SetNextItemWidth(s_Context.m_InputWidth);
			label_end = strlen(label_end) == 0 ? label : label_end;
			ret = ImGui::DragFloat(label_end, &val, speed, min, max, s_Context.m_InputFormat);

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
					// Set to true only when value changed and input has finished 
					if (done)
					{
						*done = true;
						ret = true;
					}

					// Create cmd here
					if (saveCmd)
					{
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
			}

			return ret;
		}

		glm::vec3 Input::GetStartFloat3()
		{
			return s_Context.m_TempVec3;
		}

		glm::vec2 Input::GetStartFloat2()
		{
			return s_Context.m_TempVec2;
		}

		float Input::GetStartFloat()
		{
			return s_Context.m_TempFloat;
		}

		bool Input::InputText(const char* label, char* buf, size_t size, ImGuiInputTextFlags flags)
		{
			bool ret = false;

			const char* label_end = ImGui::FindRenderedTextEnd(label);
			SetCurrentLineTextOffset();
			ImGui::TextEx(label, label_end); ImGui::SameLine(s_Context.m_LabelOffset);

			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - s_Context.m_LabelOffset + 10.0f);
			label_end = strlen(label_end) == 0 ? label : label_end;

			ret = ImGui::InputText(label_end, buf, size, flags);

			return ret;
		}

		bool Input::InputFloat(const char* label, float* val, float step, float step_fast)
		{
			bool ret = false;

			const char* label_end = ImGui::FindRenderedTextEnd(label);
			SetCurrentLineTextOffset();
			ImGui::TextEx(label, label_end); ImGui::SameLine(s_Context.m_LabelOffset);

			ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth() - s_Context.m_LabelOffset + 10.0f);
			label_end = strlen(label_end) == 0 ? label : label_end;

			ret = ImGui::InputFloat(label, val, step, step_fast, s_Context.m_InputFormat, ImGuiInputTextFlags_EnterReturnsTrue);

			return ret;
		}

	}
}
