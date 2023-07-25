#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue
{
public:
	void enqueue(T item)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		_queue.push(item);
		_conditional_variable.notify_one();
	}

	T dequeue()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		_conditional_variable.wait(lock,
			[&]() { return _should_quit || !_queue.empty();});

		T item = _queue.front();
		_queue.pop();

		return item;
	}

	bool empty() const
	{
		return _queue.empty();
	}

	void signal_quit()
	{
		_should_quit = true;
		_conditional_variable.notify_one();
	}

	bool should_quit() const
	{
		return _should_quit;
	}

private:
	std::mutex _mutex;
	std::queue<T> _queue;
	std::atomic<bool> _should_quit;
	std::condition_variable _conditional_variable;
};
