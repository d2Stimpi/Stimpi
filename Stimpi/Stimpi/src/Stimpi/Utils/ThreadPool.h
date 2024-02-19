#pragma once

#include "Stimpi/Core/Core.h"
#include "Stimpi/Log.h"

#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <future>

/**
 * From: https://www.youtube.com/watch?v=6re5U82KwbY
 */

namespace Stimpi
{
	class ThreadPool;

	// TODO: move to other class (ThreadPools or something)
	// Global resource loading threadPool
	extern ST_API std::unique_ptr<ThreadPool> s_LoaderThreadPool;

	class ST_API ThreadPool
	{
	public:
		ThreadPool() : ThreadPool(10) {}

		ThreadPool(uint32_t poolSize)
		{
			for (int i = 0; i < poolSize; i++)
			{
				auto thread = ThreadWorker(this, i);
				m_Threads.emplace_back(std::move(thread));
			}
		}

		~ThreadPool()
		{
			m_Shutdown = true;
			m_ConditionVariable.notify_all();
			for (auto& thread : m_Threads)
			{
				thread.join();
			}
		}

		template <typename T, typename... Args>
		auto AddTask(T&& t, Args&&... args) -> std::future<decltype(t(args...))>
		{
			auto func = std::bind(std::forward<T>(t), std::forward<Args>(args)...);

			// Encapsulate in shared ptr in order to be able to copy ctr / assign
			auto taskPtr = std::make_shared<std::packaged_task<decltype(t(args...))()>>(func);

			// Wrap the task pointer into a void lambda
			auto wrapperFunc = [taskPtr]() { (*taskPtr)(); };

			//std::lock_guard<std::mutex> lock(m_Mutex);
			m_TaskQueue.push(wrapperFunc);
			// Wake up one thread if it is waiting
			m_ConditionVariable.notify_one();

			// Return future from promise
			return taskPtr->get_future();
		}


		class ThreadWorker
		{
		public:
			ThreadWorker(ThreadPool* threadPool, uint32_t id) : m_ThreadPool(threadPool), m_ID(id) {}

			void operator()()
			{
				std::unique_lock<std::mutex> lock(m_ThreadPool->m_Mutex);

				while (!m_ThreadPool->m_Shutdown || (m_ThreadPool->m_Shutdown && !m_ThreadPool->m_TaskQueue.empty()))
				{
					//ST_CORE_INFO("- thread worker {} waiting", m_ID);
					m_ThreadPool->m_ConditionVariable.wait(lock, [this] {
						return this->m_ThreadPool->m_Shutdown || !this->m_ThreadPool->m_TaskQueue.empty();
					});

					if (!this->m_ThreadPool->m_TaskQueue.empty())
					{
						auto func = m_ThreadPool->m_TaskQueue.front();
						m_ThreadPool->m_TaskQueue.pop();

						lock.unlock();
						func();
						lock.lock();
					}
				}
			}

		private:
			uint32_t m_ID;
			ThreadPool* m_ThreadPool;
		};

		static void InitResourceLoadingThreadPool()
		{
			if (s_LoaderThreadPool == nullptr)
			{
				s_LoaderThreadPool = std::make_unique<ThreadPool>(10);
			}
		}

	private:
		mutable std::mutex m_Mutex;
		std::condition_variable m_ConditionVariable;

		bool m_Shutdown = false;
		std::vector<std::thread> m_Threads;

		std::queue<std::function<void()>> m_TaskQueue;
	};
}