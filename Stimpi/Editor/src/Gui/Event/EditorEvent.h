#pragma once

#include "Stimpi/Log.h"
#include "Stimpi/Core/Event.h"

namespace Stimpi
{
	enum class EditorEventType { NONE = 0, PREFAB_INSPECT_REQUEST };

	using EditorEventData = void*;

	/**
	 * TODO: create Class that pushes events on the EventQueue and processes them somewhere (EditorLayer)
	 */
	class EditorEvent : public Event
	{
	public:
		EditorEvent()
			: Event(EventType::EditorEvent), m_Type(EditorEventType::NONE), m_Data(nullptr) {}
		EditorEvent(EditorEventType type, EditorEventData data) 
			: Event(EventType::EditorEvent), m_Type(type), m_Data(data) {}
		~EditorEvent() {}

		void LogEvent() override { ST_INFO("Logging EditorEvent"); }

		static EditorEvent* Create(EditorEventType type, EditorEventData data);
		
		EditorEventType GetType() { return m_Type; }
		static EventType GetStaticType() { return EventType::EditorEvent; }

		EditorEventData GetData() { return m_Data; }

	private:
		EditorEventType m_Type{ EditorEventType::NONE };
		EditorEventData m_Data{ nullptr };
	};

}