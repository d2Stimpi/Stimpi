#pragma once

#include "Stimpi/Core/UUID.h"

#include "ImGui/src/imgui.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	using NNodeId = UUID;

	class NNode
	{
	public:
		enum class Type { None = 0, Getter, Variable, Modifier, Setter };
	
		NNode() = delete;
		NNode(const NNode&) = default;
		NNode(const std::string& title, Type type)
			: m_Title(title), m_Type(type)
		{ }

		~NNode();

		std::string& GetTitle() { return m_Title; }

		void SetPos(ImVec2 pos) { m_Pos = pos; }
		ImVec2 GetPos() { return m_Pos; }
		void Translate(ImVec2 pos) { m_Pos.x += pos.x; m_Pos.y += pos.y; }

		void SetSize(ImVec2 size) { m_Size = size; }
		ImVec2 GetSize() { return m_Size; }

		NNodeId GetID() { return m_ID; }
		bool HasHeader() { return m_HasHeader; }

		void SetSelected(bool selected) { m_IsSelected = selected; }
		bool IsSelected() { return m_IsSelected; }

		//TODO: move to style or something
		ImVec2 CalcNodeSize();

	private:
		ImVec2 m_Pos;
		ImVec2 m_Size;
		NNodeId m_ID;

		std::string m_Title = "Sample node";
		bool m_HasHeader = true;
		bool m_IsSelected;

		Type m_Type = Type::None;
	};
}