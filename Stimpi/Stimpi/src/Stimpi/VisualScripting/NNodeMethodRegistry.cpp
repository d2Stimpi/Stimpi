#include "stpch.h"
#include "Stimpi/VisualScripting/NNodeMethodRegistry.h"

#include "Stimpi/VisualScripting/ExecTree.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Log.h"

#include <glm/glm.hpp>

namespace Stimpi
{

	// TODO: NodeVerifier should validate that implemented methods have all the required components to eliminate all the checks here (performance huh)

	/**
	 * ipiN - input param index
	 * opiN - output param index
	 */

	/* Method implementations */

#pragma region GetterMethods
	
	/**
	 * Description: Provide source for Entity's QuadComponent position part.
	 * 
	 * output: glm::vec3 - QuadComponent.Pos
	 */
	static void GetPosition(Method* method)
	{
		uint32_t opi = method->m_OutParams[0];

		// Owner entity is the expected source of data
		if (method->m_ExecTree->m_Entity)
		{
			// Node verifier should make sure we have all the components on the entity that are used here
			Entity e = method->m_ExecTree->m_Entity;
			glm::vec3 pos = e.GetComponent<QuadComponent>().m_Position;

			//Set the output
			method->m_ExecTree->m_Params[opi] = pos;
		}
		else
		{
			// TODO: error?
			ST_WARN("[Node::GetPosition] no valid Entity set!");
		}

	}

#pragma endregion GetterMethods

#pragma region ModifierMethods

	/**
	 * Description: Translate Entity's QuadComponent position by provided input vector.
	 *
	 * input: glm::vec3 - input position
	 * input: glm::vec3 - translation vector
	 * output: glm::vec3 - translated position result
	 */
	static void Translate(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		glm::vec3 pos = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi1]);
		glm::vec3 vector = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = pos + vector;
	}

#pragma endregion ModifierMethods

#pragma region SetterMethods

	/**
	 * Description: Set Entity's QuadComponent position.
	 *
	 * input: glm::vec3 - input position
	 */
	static void SetPosition(Method* method)
	{
		uint32_t ipi = method->m_InParams[0];

		glm::vec3 pos = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi]);

		if (method->m_ExecTree->m_Entity)
		{
			Entity e = method->m_ExecTree->m_Entity;
			e.GetComponent<QuadComponent>().m_Position = pos;
		}
	}

#pragma endregion SetterMethods



	/* NNodeMethodRegistry */


	MethodRegistry s_MethodRegistry;

	void NNodeMethodRegistry::PopulateMethods()
	{
		/* Getters */
		RegisterMethod(MethodName::GetPosition, GetPosition);

		/* Modifiers */
		RegisterMethod(MethodName::Translate, Translate);

		/* Setters */
		RegisterMethod(MethodName::SetPosition, SetPosition);
	}

	void NNodeMethodRegistry::RegisterMethod(MethodName name, MethodType method)
	{
		s_MethodRegistry[name] = method;
	}

	MethodType NNodeMethodRegistry::GetMethod(MethodName name)
	{
		return s_MethodRegistry.at(name);
	}

}