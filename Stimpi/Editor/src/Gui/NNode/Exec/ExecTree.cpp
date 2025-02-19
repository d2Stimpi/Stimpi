#include "stpch.h"
#include "Gui/NNode/Exec/ExecTree.h"

#include "Gui/NNode/Exec/NNodeMethodRegistry.h"

namespace Stimpi
{

	Method::Method(std::vector<uint32_t> inParamIds, std::vector<uint32_t> outParamIds, MethodName name, ExecTree* owner)
		: m_Name(name), m_ExecTree(owner)
	{
		for (auto& in : inParamIds)
			m_InParams.push_back(in);

		for (auto& out : outParamIds)
			m_OutParams.push_back(out);

		m_Method = NNodeMethodRegistry::GetMethod(m_Name);
	}

	Method::Method(std::vector<uint32_t> params, bool isInput, MethodName name, ExecTree* owner)
		: m_Name(name), m_ExecTree(owner)
	{
		if (isInput)
		{
			for (auto& param : params)
				m_InParams.push_back(param);
		}
		else
		{
			for (auto& param : params)
				m_OutParams.push_back(param);
		}

		m_Method = NNodeMethodRegistry::GetMethod(m_Name);
	}

	void Method::Execute()
	{
		if (m_Method)
		{
			m_Method(this);
		}
	}

	void ExecTree::ExecuteWalk(Entity entity)
	{
		m_Entity = entity;

		for (auto& method : m_Methods)
		{
			method.Execute();
		}
	}

}