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

		static void PushEvent(std::shared_ptr<TEvent> event)
		{
			mEvents.push_back(event);
		}

		static std::vector<std::shared_ptr<TEvent>>& GetEvents()
		{
			return mEvents;
		}

		static void Flush()
		{
			mEvents.clear();
		}
		
	private:
		static std::vector<std::shared_ptr<TEvent>> mEvents;
	};

	template <typename TEvent>
	std::vector<std::shared_ptr<TEvent>> EventQueue<TEvent>::mEvents;
}