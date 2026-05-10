#pragma once
// ══════════════════════════════════════════════════════════════════
//  EventQueue.h  — Template Priority Queue for Game Events
//
//  Concepts demonstrated:
//    • Templates          — generic EventQueue<T> works with any type
//    • Priority Queue     — events scheduled by time, earliest first
//    • STL containers     — uses std::vector as underlying storage
//    • Friend class       — Tower is a friend so EventQueue can
//                           read private Tower data (hp, position)
//
//  Usage:
//    EventQueue<SpawnEvent> spawnQ;
//    spawnQ.push({ 2.5f, SpawnType::FAST });
//    if (!spawnQ.empty() && spawnQ.top().time <= elapsed)
//        process(spawnQ.pop());
// ══════════════════════════════════════════════════════════════════
#include <vector>
#include <algorithm>
#include <stdexcept>

// ── Generic game event wrapper ────────────────────────────────────
// T is the payload — e.g. SpawnEvent, DamageEvent, etc.
template<typename T>
struct TimedEvent
{
    float time;   // when this event should fire (game time in seconds)
    T     data;

    // Comparison for min-heap (earliest time = highest priority)
    bool operator>(const TimedEvent& rhs) const { return time > rhs.time; }
    bool operator<(const TimedEvent& rhs) const { return time < rhs.time; }
};

// ── EventQueue — min priority queue ordered by event time ────────
template<typename T>
class EventQueue
{
public:
    EventQueue() = default;

    // ── Push a new event into the queue ──────────────────────────
    void push(float time, T data)
    {
        heap.push_back({ time, std::move(data) });
        // Bubble up (min-heap: smallest time at top)
        std::push_heap(heap.begin(), heap.end(),
            [](const TimedEvent<T>& a, const TimedEvent<T>& b)
            { return a.time > b.time; });   // lambda (Advanced C++ req)
    }

    // ── Peek at the soonest event ─────────────────────────────────
    const TimedEvent<T>& top() const
    {
        if (heap.empty())
            throw std::runtime_error("EventQueue::top() called on empty queue");
        return heap.front();
    }

    // ── Remove and return the soonest event ───────────────────────
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

    // ── Sorting utility (Sorting Algorithm requirement) ───────────
    // Returns all pending events sorted earliest→latest (does NOT pop)
    std::vector<TimedEvent<T>> sorted() const
    {
        std::vector<TimedEvent<T>> copy = heap;
        std::sort(copy.begin(), copy.end(),
            [](const TimedEvent<T>& a, const TimedEvent<T>& b)
            { return a.time < b.time; });
        return copy;
    }

private:
    std::vector<TimedEvent<T>> heap;  // STL vector as heap backing store
};

// ── Concrete event types ──────────────────────────────────────────
struct SpawnEvent
{
    int   wave;       // which wave number
    float hp;         // enemy HP for this spawn
    float speed;      // enemy speed
    float damage;     // enemy attack damage
};