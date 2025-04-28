#include "stpch.h"
#include "Stimpi/Cmd/Commands.h"

#include "Stimpi/Log.h"
#include "Stimpi/Scene/Component.h"
#include "Stimpi/Scene/EntityManager.h"

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


	/**
	 * EntityHierarcyCommand implementation
	 */


	void EntityHierarchyCommand::Undo()
	{
		switch (m_Type)
		{
		case Stimpi::HierarchyCommandType::TRANSLATE:
			ST_CORE_ASSERT_MSG(!std::holds_alternative<glm::vec3>(m_Value), "Wrong value type for HierarchyCommandType::TRANSLATE undo command!");
			Translate(std::get<glm::vec3>(m_Value));
			break;
		case Stimpi::HierarchyCommandType::SCALE:
			ST_CORE_ASSERT_MSG(!std::holds_alternative<glm::vec2>(m_Value), "Wrong value type for HierarchyCommandType::SCALE undo command!");
			Scale(std::get<glm::vec2>(m_Value));
			break;
		case Stimpi::HierarchyCommandType::ROTATE:
			ST_CORE_ASSERT_MSG(!std::holds_alternative<float>(m_Value), "Wrong value type for HierarchyCommandType::ROTATE undo command!");
			Rotate(std::get<float>(m_Value));
			break;
		default:
			break;
		}
	}

	void EntityHierarchyCommand::Redo()
	{
		switch (m_Type)
		{
		case Stimpi::HierarchyCommandType::TRANSLATE:
			ST_CORE_ASSERT_MSG(!std::holds_alternative<glm::vec3>(m_Value), "Wrong value type for HierarchyCommandType::TRANSLATE redo command!");
			Translate(std::get<glm::vec3>(m_Value), false);
			break;
		case Stimpi::HierarchyCommandType::SCALE:
			ST_CORE_ASSERT_MSG(!std::holds_alternative<glm::vec2>(m_Value), "Wrong value type for HierarchyCommandType::SCALE redo command!");
			Scale(std::get<glm::vec2>(m_Value), false);
			break;
		case Stimpi::HierarchyCommandType::ROTATE:
			ST_CORE_ASSERT_MSG(!std::holds_alternative<float>(m_Value), "Wrong value type for HierarchyCommandType::ROTATE redo command!");
			Rotate(std::get<float>(m_Value), false);
			break;
		default:
			break;
		}
	}

	EntityHierarchyCommand* EntityHierarchyCommand::Create(Entity entity, HierarchyCommandType type, EntityValueVariant value)
	{
		return new EntityHierarchyCommand(entity, type, value);
	}

	void EntityHierarchyCommand::Translate(const glm::vec3& vec, bool undo)
	{
		if (undo)
			EntityManager::Translate(m_Entity, -vec);
		else
			EntityManager::Translate(m_Entity, vec);
	}

	void EntityHierarchyCommand::Scale(const glm::vec2& vec, bool undo)
	{
		if (undo)
			EntityManager::Scale(m_Entity, -vec);
		else
			EntityManager::Scale(m_Entity, vec);
	}

	void EntityHierarchyCommand::Rotate(const float& val, bool undo)
	{

		if (undo)
			EntityManager::Rotate(m_Entity, -val);
		else
			EntityManager::Rotate(m_Entity, val);
	}

}