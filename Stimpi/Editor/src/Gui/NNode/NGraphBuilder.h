#pragma once

#include "Gui/NNode/NGraph.h"

#include "Stimpi/VisualScripting/NNodeMethodRegistry.h"

namespace Stimpi
{
	struct NGraphBuildInfo
	{
		std::string m_Name = "Unnamed";
		std::vector<MethodName> m_Methods;

		NGraphBuildInfo(const std::string& name, std::initializer_list<MethodName> methods)
			: m_Name(name)
		{
			for (auto& method : methods)
				m_Methods.push_back(method);
		}
	};

	class NGraphBuilder
	{
	public:
		static std::shared_ptr<NGraph> BuildGraph(const NGraphBuildInfo& buildInfo);

		NGraphBuilder Create(const std::string& name);
		NGraphBuilder Create(const NGraphBuildInfo& buildInfo);
		NGraphBuilder AddNode(std::shared_ptr<NNode> node);

		std::shared_ptr<NGraph> GetGraph() { return m_Graph; }
		operator std::shared_ptr<NGraph>() { return m_Graph; }

	private:
		std::shared_ptr<NGraph> m_Graph;
		ImVec2 m_NewNodePos;
	};
}