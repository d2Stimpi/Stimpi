#pragma once

#include "Stimpi/Core/UUID.h"

#include "ImGui/src/imgui.h"
#include <glm/glm.hpp>

namespace Stimpi
{
	using NNodeId = UUID;
	using NPinId = UUID;

	struct NNode;

	struct NPin
	{
		enum class Type { None = 0, In, Out };

		NPin() = delete;
		NPin(const NPin&) = default;
		NPin(NNode* parent, Type type, const std::string& label)
			: m_Type(type), m_Label(label)
		{
			m_ParentNode.reset(parent);
		}

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

	struct NPinConnection
	{
		ImVec2 CalcFirstMidBezierPoint(const ImVec2& start, const ImVec2& end);
		ImVec2 CalcLastMidBezierPoint(const ImVec2& start, const ImVec2& end);
		ImVec2 BezierCubicCalc(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, const ImVec2& p4, float t);
		void CalculateBezierPoints(uint32_t segments);
		void UpdateConnectionPoints();

		std::shared_ptr<NPin> m_Src;
		std::shared_ptr<NPin> m_Dest;

		std::vector<ImVec2> m_BezierLinePoints;

		NPinConnection() = delete;
		NPinConnection(std::shared_ptr<NPin> src, std::shared_ptr<NPin> dest)
			: m_Src(src), m_Dest(dest)
		{ }
	};

	struct NNode
	{
		enum class Type { None = 0, Getter, Variable, Modifier, Setter };

		NNode() = delete;
		NNode(const NNode&) = default;
		NNode(const std::string& title, Type type)
			: m_Title(title), m_Type(type)
		{
		}

		~NNode();

		ImVec2 CalcNodeSize();

		void AddPin(NPin::Type type, const std::string& title);

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
}