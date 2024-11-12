#pragma once

#include "Stimpi/Cmd/Command.h"

namespace Stimpi
{
	using EntityValueVariant = std::variant<glm::vec4, glm::vec3, glm::vec2, float, uint32_t, bool>;

	class ST_API EntityCommand : public Command
	{
	public:
		EntityCommand(Entity entity, EntityValueVariant value, void* ptr)
			: Command(CommandType::TRANSLATE), m_Entity(entity), m_Value(value), m_DataPtr(ptr)
		{}

		void Undo() override;
		void Redo() override;

		static CommandType GetType() { return CommandType::TRANSLATE; }
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
}