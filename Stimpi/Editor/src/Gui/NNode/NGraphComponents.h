#pragma once

//#include "Stimpi/VisualScripting/NNodeMethodRegistry.h"
#include "Stimpi/VisualScripting/ExecTree.h"
#include "Stimpi/Core/UUID.h"
#include "Stimpi/Log.h"

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
		enum class DataType { Any = 0, Entity, Bool, Int, Int2, Int3, Int4, Float, Float2, Float3, Float4 };

		NPin() = delete;
		NPin(const NPin&) = default;
		NPin(NNode* parent, const Type& type, const std::string& label, const DataType& dataType)
			: m_Type(type), m_Label(label), m_DataType(dataType)
		{
			m_ParentNode = parent;
		}

		float GetPinSpaceHeight();
		float GetPinSpaceWidth();

		Type m_Type;
		std::string m_Label = "Sample pin";
		NNode* m_ParentNode; // Owner node - TODO: rename to m_Node

		NPinId m_ID;
		bool m_Connected = false;
		bool m_SingleConnection = false;
		std::vector<std::shared_ptr<NPin>> m_ConnectedPins;

		ImVec2 m_Pos = { 0.0f, 0.0f };

		// Pin data type
		DataType m_DataType = DataType::Any;
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

		ImVec2 CalcNodeSize();

		void AddPin(NPin::Type type, const std::string& title, const NPin::DataType& dataType = NPin::DataType::Any);
		void AddMethod(const MethodName& methodName) { m_MethodName = methodName; }
		
		// TODO: consider making this as a local value
		bool HasConnection();
		bool HasInputConnection();
		bool HasOutputConnection();

		ImVec2 m_Pos;
		ImVec2 m_Size;
		NNodeId m_ID;

		std::string m_Title = "Sample node";
		bool m_HasHeader = true;

		Type m_Type = Type::None;

		// List of Pins
		std::vector<std::shared_ptr<NPin>> m_InPins;
		std::vector<std::shared_ptr<NPin>> m_OutPins;

		// Modifier, Getter or Setter method name
		MethodName m_MethodName = MethodName::None;
	};
}