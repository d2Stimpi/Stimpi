#pragma once

#include "Stimpi/Core/UUID.h"

#include "ImGui/src/imgui.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	using NNodeId = UUID;

	class NNode
	{
		enum class Type { None = 0, Getter, Variable, Modifier, Setter };
	
	public:
		NNode() = default;
		NNode(const NNode&) = default;
		NNode(const ImVec2 & pos) : m_Pos(pos) {}
		~NNode();

		virtual void OnImGuiRender();

		void SetPos(ImVec2 pos) { m_Pos = pos; }
		ImVec2 GetPos() { return m_Pos; }

		void SetSize(ImVec2 size) { m_Size = size; }
		ImVec2 GetSize() { return m_Size; }

		//TODO: move to style or something
		ImVec2 CalcNodeSize();

	private:
		ImVec2 m_Pos;
		ImVec2 m_Size;
		NNodeId m_ID;

		std::string m_Title = "Sample node";
		bool m_HasHeader = true;

		Type m_Type = Type::None;
	};
}