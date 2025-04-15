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
	static bool GetEntity(Method* method)
	{
		return true;
	}

	/**
	 * Description: Provide source for Entity's QuadComponent position data.
	 * 
	 * output: glm::vec3 - QuadComponent.Pos
	 */
	static bool GetPosition(Method* method)
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
			ST_WARN("[Node::GetPosition] no valid Entity set!");
			return false;
		}

		return true;
	}

	/**
	 * Description: Provide source for Entity's QuadComponent position data.
	 *
	 * output: glm::vec2 - QuadComponent.Size
	 */
	static bool GetSize(Method* method)
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
			ST_WARN("[Node::GetSize] no valid Entity set!");
			return false;
		}
		return true;
	}

	/**
	 * Description: Provide source for Entity's AnimatedSpriteComponent data.
	 *
	 * output: AnimSprite - AnimatedSpriteComponent.AnimSprite
	 * output: DefaultAnimation - AnimatedSpriteComponent.DefaultAnimation
	 * output: Animations - AnimatedSpriteComponent.Animations
	 * output: Material - AnimatedSpriteComponent.Material
	 */
	static bool GetAnimationComponent(Method* method)
	{
		uint32_t opi1 = method->m_OutParams[0];
		uint32_t opi2 = method->m_OutParams[1];
		uint32_t opi3 = method->m_OutParams[2];
		uint32_t opi4 = method->m_OutParams[3];

		// Owner entity is the expected source of data
		if (method->m_ExecTree->m_Entity)
		{
			// Node verifier should make sure we have all the components on the entity that are used here
			Entity e = method->m_ExecTree->m_Entity;
			if (e.HasComponent<AnimatedSpriteComponent>())
			{
				AnimatedSpriteComponent& component = e.GetComponent<AnimatedSpriteComponent>();

				method->m_ExecTree->m_Params[opi1] = component.m_AnimSprite.get();
				method->m_ExecTree->m_Params[opi2] = component.m_DefaultAnimation;
				method->m_ExecTree->m_Params[opi3] = &component.m_Animations;
				method->m_ExecTree->m_Params[opi4] = component.m_Material.get();
			}
			else
			{
				ST_WARN("[Node::GetAnimationComponent] entity has no AnimatedSpriteComponent!");
				return false;
			}
		}
		else
		{
			ST_WARN("[Node::GetSize] no valid Entity set!");
			return false;
		}
		return true;
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
	static bool Translate(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		//glm::vec3 pos = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi1]);
		//glm::vec3 vector = std::visit([](auto&& arg) { return (glm::vec3)arg; }, method->m_ExecTree->m_Params[ipi2]);
		glm::vec3 pos = std::get<glm::vec3>(method->m_ExecTree->m_Params[ipi1]);
		glm::vec3 vector = std::get<glm::vec3>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = pos + vector;

		return true;
	}

	/**
	 * Description: Add operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A + B
	 */
	static bool Add(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = a + b;

		return true;
	}

	/**
	 * Description: Subtract operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A - B
	 */
	static bool Subtract(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = a - b;

		return true;
	}

	/**
	 * Description: Divide operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A / B
	 */
	static bool Divide(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		if (a != 0)
			method->m_ExecTree->m_Params[opi] = a / b;
		else
		{
			method->m_ExecTree->m_Params[opi] = 0.0f;
			ST_WARN("[Node::Divide] dividing by 0.0!");
			return false;
		}

		return true;
	}

	/**
	 * Description: Multiply operation.
	 *
	 * input: float - A
	 * input: float - B
	 * output: float - A * B
	 */
	static bool Multiply(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t ipi2 = method->m_InParams[1];
		uint32_t opi = method->m_OutParams[0];

		float a = std::get<float>(method->m_ExecTree->m_Params[ipi1]);
		float b = std::get<float>(method->m_ExecTree->m_Params[ipi2]);

		method->m_ExecTree->m_Params[opi] = a * b;

		return true;
	}

	/**
	 * Description: Break Vector2 to X and Y components
	 *
	 * input: glm::vec2 - input vector
	 * output: float - X component
	 * output: float - Y component
	 */
	static bool Vector2(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t opi1 = method->m_OutParams[0];
		uint32_t opi2 = method->m_OutParams[1];

		glm::vec2 pos = std::get<glm::vec2>(method->m_ExecTree->m_Params[ipi1]);
		
		method->m_ExecTree->m_Params[opi1] = pos.x;
		method->m_ExecTree->m_Params[opi2] = pos.y;

		return true;
	}

	/**
	 * Description: Decomposition of AnimSprite type
	 * 
	 * input: AnimSprite - Animated sprite asset
	 * output: Animation - Animation asset ref
	 * output: float - playback speed
	 */
	static bool AnimSpriteMod(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t opi1 = method->m_OutParams[0];
		uint32_t opi2 = method->m_OutParams[1];
		uint32_t opi3 = method->m_OutParams[2];

		AnimatedSprite* animSprite = static_cast<AnimatedSprite*>(std::get<void*>(method->m_ExecTree->m_Params[ipi1]));

		method->m_ExecTree->m_Params[opi1] = animSprite->GetAnimation().get();
		method->m_ExecTree->m_Params[opi2] = animSprite->GetPlaybackSpeed();
		method->m_ExecTree->m_Params[opi3] = animSprite->GetSubTexture();

		return true;
	}

	/**
	 * Description: Decomposition of Animation type
	 * 
	 * input: Animation - Animation ref
	 * output:  TODO
	 */
	static bool AnimationMod(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		//uint32_t opi1 = method->m_OutParams[0];

		Animation* animation = static_cast<Animation*>(std::get<void*>(method->m_ExecTree->m_Params[ipi1]));
		
		//method->m_ExecTree->m_Params[opi1] = animation->GetSubTexture();

		return true;
	}

	/**
	 * Description: Decomposition of Animation type
	 *
	 * input: SubTexture - SubTexture ref
	 * output: glm::vec2 - SubTexture frame size (width, height)
	 */
	static bool SubTextureMod(Method* method)
	{
		uint32_t ipi1 = method->m_InParams[0];
		uint32_t opi1 = method->m_OutParams[0];

		SubTexture* subTexture = static_cast<SubTexture*>(std::get<void*>(method->m_ExecTree->m_Params[ipi1]));

		//method->m_ExecTree->m_Params[opi1] = glm::vec2(subTexture->GetSubWidht(), subTexture->GetSubHeight());
		method->m_ExecTree->m_Params[opi1] = glm::vec2(24.0f, 24.0f);

		return true;
	}

#pragma endregion ModifierMethods

#pragma region SetterMethods

	/**
	 * Description: Set Entity's QuadComponent position.
	 *
	 * input: glm::vec3 - input position
	 */
	static bool SetPosition(Method* method)
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

		return true;
	}

	static bool SetShaderData(Method* method)
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

		return true;
	}

#pragma endregion SetterMethods



	/* NNodeMethodRegistry */


	MethodRegistry s_MethodRegistry;

	void NNodeMethodRegistry::PopulateMethods()
	{
		// Load methods if it was not done already - App vs Project loading
		if (!s_MethodRegistry.empty())
			return;

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