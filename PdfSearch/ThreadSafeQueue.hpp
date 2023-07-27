#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue final
{
public:
	ThreadSafeQueue() :
		_should_quit(false)
	{}

public:
	void enqueue(const T& item)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_queue.push(item);

		lock.unlock();
		_conditional_variable.notify_one();
	}

	T dequeue()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_conditional_variable.wait(lock, [this] { return !_queue.empty() || _should_quit; });

		if (_queue.empty())
		{
			throw std::runtime_error("Quit signaled during dequeue");
		}

		T front = _queue.front();
		_queue.pop();

		return front;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lock_guard{ _mutex };
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
	mutable std::mutex _mutex;
	std::queue<T> _queue;
	std::atomic<bool> _should_quit;
	std::condition_variable _conditional_variable;
};
