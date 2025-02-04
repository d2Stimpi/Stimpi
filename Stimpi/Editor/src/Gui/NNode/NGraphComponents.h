#pragma once

#include "Stimpi/Core/UUID.h"

#include "ImGui/src/imgui.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	using NNodeId = UUID;
	using NPinId = UUID;

	struct NPin;

	struct NNode
	{
		enum class Type { None = 0, Getter, Variable, Modifier, Setter };
	
		NNode() = delete;
		NNode(const NNode&) = default;
		NNode(const std::string& title, Type type)
			: m_Title(title), m_Type(type)
		{ }

		~NNode();

		ImVec2 CalcNodeSize();

		ImVec2 m_Pos;
		ImVec2 m_Size;
		NNodeId m_ID;

		std::string m_Title = "Sample node";
		bool m_HasHeader = true;

		Type m_Type = Type::None;

		// List of Pins
		std::vector<std::shared_ptr<NPin>> m_InPins;
		std::vector<std::shared_ptr<NPin>> m_OutPins;
	};

	struct NPin
	{
		enum class Type { None = 0, In, Out };

		NPin() = delete;
		NPin(const NPin&) = default;
		NPin(std::shared_ptr<NNode> parent, Type type, const std::string& label)
			: m_Type(type), m_ParentNode(parent), m_Label(label)
		{}

		float GetPinSpaceHeight();
		float GetPinSpaceWidth();

		Type m_Type;
		std::string m_Label = "Sample pin";
		std::shared_ptr<NNode> m_ParentNode;

		NPinId m_ID;
		bool m_Connected = false;
		bool m_SingelConnection = false;
		std::vector<std::shared_ptr<NPin>> m_ConnectedPins;

		ImVec2 m_Pos = { 0.0f, 0.0f };

	};
}