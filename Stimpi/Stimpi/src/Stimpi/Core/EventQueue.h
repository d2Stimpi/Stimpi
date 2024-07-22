#pragma once

#include "Stimpi/Core/Event.h"

namespace Stimpi
{
	/**
	 * Event Queue
	 * - Per frame processing
	 * - Template - no need for more sorting based on type
	 * - TODO: 
	 *		- Consider Event priority
	 *		- Threading
	 */

	template <typename TEvent>
	class EventQueue
	{
	public:

		static void Reserve(size_t size)
		{
			m_Events.reserve(size);
		}

		static void PushEvent(std::shared_ptr<TEvent> event)
		{
			m_Events.push_back(event);
		}

		static std::vector<std::shared_ptr<TEvent>>& GetEvents()
		{
			return m_Events;
		}

		static void Flush()
		{
			m_Events.clear();
		}
		
	private:
		static std::vector<std::shared_ptr<TEvent>> m_Events;
	};

	template <typename TEvent>
	std::vector<std::shared_ptr<TEvent>> EventQueue<TEvent>::m_Events;
}