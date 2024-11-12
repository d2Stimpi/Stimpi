#include "stpch.h"
#include "Stimpi/Cmd/Commands.h"

#include "Stimpi/Scene/Component.h"

#include <glm/gtc/type_ptr.hpp>

namespace Stimpi
{

	void EntityCommand::Undo()
	{
		std::visit([&](auto&& arg) { SetVariantValue(arg); }, m_Value);
	}

	void EntityCommand::Redo()
	{
		std::visit([&](auto&& arg) { SetVariantValue(arg, false); }, m_Value);
	}

	EntityCommand* EntityCommand::Create(Entity entity, EntityValueVariant value, void* ptr)
	{
		return new EntityCommand(entity, value, ptr);
	}

	EntityValueVariant& EntityCommand::GetValue()
	{
		return m_Value;
	}

	void EntityCommand::SetVariantValue(glm::vec4 value, bool undo)
	{
		if (m_DataPtr)
		{
			if (undo)
				*(glm::vec4*)m_DataPtr -= value;
			else
				*(glm::vec4*)m_DataPtr += value;
		}
	}

	void EntityCommand::SetVariantValue(glm::vec3 value, bool undo)
	{
		if (m_DataPtr)
		{
			if (undo)
				*(glm::vec3*)m_DataPtr -= value;
			else
				*(glm::vec3*)m_DataPtr += value;
		}
	}

	void EntityCommand::SetVariantValue(glm::vec2 value, bool undo)
	{
		if (m_DataPtr)
		{
			if (undo)
				*(glm::vec2*)m_DataPtr -= value;
			else
				*(glm::vec2*)m_DataPtr += value;
		}
	}

	void EntityCommand::SetVariantValue(float value, bool undo)
	{
		if (m_DataPtr)
		{
			if (undo)
				*(float*)m_DataPtr -= value;
			else
				*(float*)m_DataPtr += value;
		}
	}

	void EntityCommand::SetVariantValue(uint32_t value, bool undo)
	{
		if (m_DataPtr)
		{
			if (undo)
				*(uint32_t*)m_DataPtr -= value;
			else
				*(uint32_t*)m_DataPtr += value;
		}
	}

	void EntityCommand::SetVariantValue(bool value, bool undo)
	{
		if (m_DataPtr)
		{
			*(bool*)m_DataPtr = undo ? value : !value;
		}
	}

}