#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Scene/Entity.h"

#include <glm/glm.hpp>

// TODO: move to Engine core and make it an asset

namespace Stimpi
{
	// Forwards
	struct Method;
	struct ExecTree;

	enum class MethodName
	{ 
		None,
		GetEntity,
		GetPosition, Translate, SetPosition
	};

	using Param = std::variant<uint32_t, bool, int, float, glm::vec3>;
	using MethodType = std::function<void(Method*)>;

	struct ST_API Method
	{
		ExecTree* m_ExecTree;

		// Index of parameters in ExecTree
		std::vector<uint32_t> m_InParams;	
		std::vector<uint32_t> m_OutParams;

		MethodName m_Name;
		MethodType m_Method;

		Method(std::vector<uint32_t> inParamIds, std::vector<uint32_t> outParamIds, MethodName name, ExecTree* owner);
		Method(std::vector<uint32_t> params, bool isInput, MethodName name, ExecTree* owner);
		void Execute();
	};

	struct ST_API ExecTree
	{
		Entity m_Entity = 0;

		std::vector<Param> m_Params;
		std::vector<Method> m_Methods;

		// For reusability of same Tree on multiple Entities
		void ExecuteWalk(Entity entity);
	};
}