#include "stpch.h"
#include "Stimpi/Cmd/Commands.h"

#include "Stimpi/Scene/Component.h"

namespace Stimpi
{

	void TranslateCommand::Execute()
	{
		auto& quad = m_Entity.GetComponent<QuadComponent>();
		quad.m_Position -= m_Value;
	}

	TranslateCommand* TranslateCommand::Create(Entity entity, glm::vec3 value)
	{
		return new TranslateCommand(entity, value);
	}

}