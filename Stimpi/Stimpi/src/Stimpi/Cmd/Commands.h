#pragma once

#include "Stimpi/Cmd/Command.h"

namespace Stimpi
{
	using EntityValueVariant = std::variant<glm::vec4, glm::vec3, glm::vec2, float, uint32_t, bool>;

	class ST_API EntityCommand : public Command
	{
	public:
		EntityCommand(Entity entity, EntityValueVariant value, void* ptr)
			: m_Entity(entity), m_Value(value), m_DataPtr(ptr)
		{}

		void Undo() override;
		void Redo() override;

		static EntityCommand* Create(Entity entity, EntityValueVariant value, void* ptr);

		EntityValueVariant& GetValue();
	private:
		void SetVariantValue(glm::vec4 value, bool undo = true);
		void SetVariantValue(glm::vec3 value, bool undo = true);
		void SetVariantValue(glm::vec2 value, bool undo = true);
		void SetVariantValue(float value, bool undo = true);
		void SetVariantValue(uint32_t value, bool undo = true);
		void SetVariantValue(bool value, bool undo = true);

	private:
		Entity m_Entity;
		EntityValueVariant m_Value;
		void* m_DataPtr;
	};

	// Commands for recording Entity Hierarchy changes

	enum class HierarchyCommandType
	{
		TRANSLATE,
		SCALE,
		ROTATE
	};

	class ST_API EntityHierarchyCommand : public Command
	{
	public:
		EntityHierarchyCommand(Entity entity, HierarchyCommandType type, EntityValueVariant value)
			: m_Entity(entity), m_Type(type), m_Value(value)
		{}

		void Undo() override;
		void Redo() override;

		static EntityHierarchyCommand* Create(Entity entity, HierarchyCommandType type, EntityValueVariant value);

	private:
		void Translate(const glm::vec3& vec, bool undo = true);
		void Scale(const glm::vec2& vec, bool undo = true);
		void Rotate(const float& val, bool undo = true);

	private:
		Entity m_Entity;
		HierarchyCommandType m_Type;
		EntityValueVariant m_Value;
	};
}