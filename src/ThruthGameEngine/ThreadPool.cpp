#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
	: m_numThread(num_threads)
	, stop_all(false) 
{
	m_workerThread.reserve(m_numThread);

	for (size_t i = 0; i < m_numThread; ++i) 
	{
		m_workerThread.emplace_back(
			[this]() 
			{ 
				this->WorkerThread(); 
			}
		);
	}
}


ThreadPool::~ThreadPool() 
{
	stop_all = true;
	m_cv.notify_all();

	for (auto& t : m_workerThread) 
	{
		t.join();
	}
}


void ThreadPool::WorkerThread() 
{
	while (true) 
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_cv.wait(
			lock, 
			[this]() 
			{ 
				return !this->m_jobQueue.empty() || stop_all; 
			}
		);

		if (stop_all && this->m_jobQueue.empty())
		{
			return;
		}

		std::function<void()> job = std::move(m_jobQueue.front());
		m_jobQueue.pop();
		lock.unlock();

		job();
	}
}