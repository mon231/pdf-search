#pragma once

#include <mutex>
#include <atomic>
#include <queue>
#include <stdexcept>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue
{
public:
    explicit ThreadSafeQueue(size_t max_size)
        : _max_size(max_size), _should_quit(false) {}

public:
    void enqueue(const T& item)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition_queue_insert.wait(lock, [this] { return _queue.size() < _max_size || _should_quit; });

        if (_should_quit)
        {
            throw std::runtime_error("Quit signaled during insert");
        }

        _queue.push(item);
        _condition_queue_removal.notify_one();
    }

    T dequeue()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _condition_queue_removal.wait(lock, [this] { return !_queue.empty() || _should_quit; });

        if (_queue.empty())
        {
            throw std::runtime_error("Quit signaled during removal");
        }

        T item = _queue.front();
        _queue.pop();

        _condition_queue_insert.notify_one();
        return item;
    }

    void signal_quit() 
    {
        _should_quit = true;
        _condition_queue_removal.notify_all();
        _condition_queue_insert.notify_all();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    bool should_quit() const
    {
        return _should_quit;
    }

private:
    std::queue<T> _queue;
    mutable std::mutex _mutex;
    std::condition_variable _condition_queue_removal;
    std::condition_variable _condition_queue_insert;
    std::atomic<bool> _should_quit;
    const size_t _max_size;
};
