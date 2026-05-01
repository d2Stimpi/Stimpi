#include "stpch.h"

#include "Gui/Event/EditorEventQueue.h"

namespace Stimpi
{
	struct EventQueue
	{
		std::vector<Event*> m_Events{};
	};

	EventQueue s_EventQueue;

	void EditorEventQueue::PushEvent(Event* event)
	{
		if (event)
		{
			s_EventQueue.m_Events.emplace_back(event);
		}
	}

	bool EditorEventQueue::PollEvent(Event** event)
	{
		if (!s_EventQueue.m_Events.empty())
		{
			*event = s_EventQueue.m_Events.back();
			s_EventQueue.m_Events.pop_back();
			return true;
		}
		return false;
	}

}