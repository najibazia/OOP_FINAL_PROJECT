#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>

template<typename T>
struct TimedEvent
{
    float time;  
    T     data;

    bool operator>(const TimedEvent& rhs) const { return time > rhs.time; }
    bool operator<(const TimedEvent& rhs) const { return time < rhs.time; }
};

template<typename T>
class EventQueue
{
public:
    EventQueue() = default;

    void push(float time, T data)
    {
        heap.push_back({ time, std::move(data) });
        std::push_heap(heap.begin(), heap.end(),
            [](const TimedEvent<T>& a, const TimedEvent<T>& b)
            { return a.time > b.time; }); 
    }

    const TimedEvent<T>& top() const
    {
        if (heap.empty())
            throw std::runtime_error("EventQueue::top() called on empty queue");
        return heap.front();
    }
    TimedEvent<T> pop()
    {
        if (heap.empty())
            throw std::runtime_error("EventQueue::pop() called on empty queue");
        std::pop_heap(heap.begin(), heap.end(),
            [](const TimedEvent<T>& a, const TimedEvent<T>& b)
            { return a.time > b.time; });
        TimedEvent<T> ev = std::move(heap.back());
        heap.pop_back();
        return ev;
    }

    bool  empty() const { return heap.empty(); }
    int   size()  const { return (int)heap.size(); }

    std::vector<TimedEvent<T>> sorted() const
    {
        std::vector<TimedEvent<T>> copy = heap;
        std::sort(copy.begin(), copy.end(),
            [](const TimedEvent<T>& a, const TimedEvent<T>& b)
            { return a.time < b.time; });
        return copy;
    }

private:
    std::vector<TimedEvent<T>> heap; 
};
struct SpawnEvent
{
    int   wave;      
    float hp;         
    float speed;      
    float damage;    
};
