#pragma once
#include "Headers.h"
#include <thread>
#include <mutex>
#include <future>
#include <type_traits>
#include <memory>

class ThreadPool 
{
private:
	size_t m_numThread;
	std::vector<std::thread> m_workerThread;
	std::queue<std::function<void()>> m_jobQueue;
	std::condition_variable m_cv;
	std::mutex m_mutex;

	bool stop_all;

public:
	ThreadPool(size_t num_threads);
	~ThreadPool();

	template <class F, class... Args>
	std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(F f, Args... args);

public:
	void WorkerThread();
};

template <class F, class... Args>
std::future<typename std::result_of<F(Args...)>::type> ThreadPool::EnqueueJob(F f, Args... args) 
{
	if (stop_all) 
	{
		throw std::runtime_error("ThreadPool »ç¿ë ÁßÁöµÊ");
	}

	using return_type = typename std::invoke_result_t<F, Args...>;

	auto job = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

	std::future<return_type> result = job->get_future();
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_jobQueue.push(
			[job]() 
			{ 
				(*job)(); 
			}
		);
	}
	m_cv.notify_one();

	return result;
}


