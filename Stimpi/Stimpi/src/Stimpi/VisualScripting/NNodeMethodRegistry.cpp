#include "stpch.h"
#include "Stimpi/VisualScripting/NNodeMethodRegistry.h"

//#include "Stimpi/Asset/AssetManager.h"
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
	 * Description: Get owner's entity (TODO)
	 * 
	 * output: Entity
	 */
	static void GetEntity(Method* method)
	{

	}

	/**
	 * Description: Provide source for Entity's QuadComponent position data.
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

	/**
	 * Description: Provide source for Entity's QuadComponent position data.
	 *
	 * output: glm::vec2 - QuadComponent.Size
	 */
	static void GetSize(Method* method)
	{
		uint32_t opi = method->m_OutParams[0];

		// Owner entity is the expected source of data
		if (method->m_ExecTree->m_Entity)
		{
			// Node verifier should make sure we have all the components on the entity that are used here
			Entity e = method->m_ExecTree->m_Entity;
			glm::vec2 pos = e.GetComponent<QuadComponent>().m_Size;

			//Set the output
			method->m_ExecTree->m_Params[opi] = pos;
		}
		else
		{
			// TODO: error?
			ST_WARN("[Node::GetSize] no valid Entity set!");
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

		//glm::vec3 pos = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi1]);
		//glm::vec3 vector = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi2]);
		glm::vec3 pos = std::get<glm::vec3>(method->m_ExecTree->m_Params[ipi1]);
		glm::vec3 vector = std::get<glm::vec3>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = pos + vector;
	}

	/**
	 * Description: Add operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A + B
	 */
	static void Add(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = a + b;
	}

	/**
	 * Description: Subtract operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A - B
	 */
	static void Subtract(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = a - b;
	}

	/**
	 * Description: Divide operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A / B
	 */
	static void Divide(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		if (a != 0)
			method->m_ExecTree->m_Params[opi] = a / b;
		else
			method->m_ExecTree->m_Params[opi] = 0.0f;
		// TODO: report error or handle invalid division
	}

	/**
	 * Description: Multiply operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A * B
	 */
	static void Multiply(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = a * b;
	}

	/**
	 * Description: Break Vector2 to X and Y components
	 *
	 * input: glm::vec2 - input vector
	 * output: float - X component
	 * output: float - Y component
	 */
	static void Vector2(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t opi1 = method->m_OutParams[0];
		uint32_t opi2 = method->m_OutParams[1];

		glm::vec2 pos = std::get<glm::vec2>(method->m_ExecTree->m_Params[ipi1]);
		
		method->m_ExecTree->m_Params[opi1] = pos.x;
		method->m_ExecTree->m_Params[opi2] = pos.y;
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

		//glm::vec3 pos = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi]);
		glm::vec3 pos = std::get<glm::vec3>(method->m_ExecTree->m_Params[ipi]);

		if (method->m_ExecTree->m_Entity)
		{
			Entity e = method->m_ExecTree->m_Entity;
			if (e.HasComponent<QuadComponent>())
				e.GetComponent<QuadComponent>().m_Position = pos;
		}
	}

	static void SetShaderData(Method* method)
	{
		if (method->m_ExecTree->m_Entity)
		{
			Entity e = method->m_ExecTree->m_Entity;
			if (e.HasComponent<AnimatedSpriteComponent>())
			{
				AnimatedSpriteComponent anim = e.GetComponent<AnimatedSpriteComponent>();
				auto shader = anim.m_Material->GetShader();
				ShaderLayout& layout = shader->GetInfo().m_ShaderLayout;
				// Skip "default" shader layouts
				size_t i = 3;

				// Go trough all input param index entries
				for (auto ipi : method->m_InParams)
				{
					// layout size must match and be in the same order as inParams
					shader_variant data;

					if (layout.m_Data[i].m_Type == ShaderDataType::Float)
						data = std::get<float>(method->m_ExecTree->m_Params[ipi]);
					else if (layout.m_Data[i].m_Type == ShaderDataType::Float2)
						data = std::get<glm::vec2>(method->m_ExecTree->m_Params[ipi]);
					else if (layout.m_Data[i].m_Type == ShaderDataType::Float3)
						data = std::get<glm::vec3>(method->m_ExecTree->m_Params[ipi]);
					else if (layout.m_Data[i].m_Type == ShaderDataType::Float4)
						data = std::get<glm::vec4>(method->m_ExecTree->m_Params[ipi]);
					else if (layout.m_Data[i].m_Type == ShaderDataType::Int)
						data = std::get<int>(method->m_ExecTree->m_Params[ipi]);
					
					shader->SetLayerData(layout.m_Data[i++].m_Name, data);
				}
			}
		}
	}

#pragma endregion SetterMethods



	/* NNodeMethodRegistry */


	MethodRegistry s_MethodRegistry;

	void NNodeMethodRegistry::PopulateMethods()
	{
#define DEFINE_NODE_NAME(name)	RegisterMethod(MethodName::name, name);
#include "Stimpi/VisualScripting/NNodeNames.h"
#undef DEFINE_NODE_NAME(name)
	}

	void NNodeMethodRegistry::RegisterMethod(MethodName name, MethodType method)
	{
		s_MethodRegistry[name] = method;
	}

	MethodType NNodeMethodRegistry::GetMethod(MethodName name)
	{
		return s_MethodRegistry.at(name);
	}

	std::string NNodeMethodRegistry::MethodNameToString(MethodName name)
	{
		switch (name)
		{
#define DEFINE_NODE_NAME(name)	case Stimpi::MethodName::name: return #name;
#include "Stimpi/VisualScripting/NNodeNames.h"
#undef DEFINE_NODE_NAME(name)
		default: return "None";
		}

		return "None";
	}

	Stimpi::MethodName NNodeMethodRegistry::StringToMethodName(const std::string& str)
	{
#define DEFINE_NODE_NAME(name)	if (str == #name) return MethodName::name;
#include "Stimpi/VisualScripting/NNodeNames.h"
#undef DEFINE_NODE_NAME(name)

		return MethodName::None;
	}

}