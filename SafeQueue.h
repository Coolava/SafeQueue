#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<class T>
class SafeQueue
{

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
    bool mIsLoop;
    size_t limit = 4096;

public:
	SafeQueue()
        : q()
        , m()
        , c()
    {}
    ~SafeQueue(void) {};

    /*Default : 4096*/
    void setLimit(size_t _limit)
    {
        limit = _limit;
    }
    void enqueue(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);

        /*for circular*/
        if ((q.size() > limit)
            && (limit > 0))
        {
            q.front();
            q.pop();
        }

        c.notify_one();

    }

    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue(void)
    {
        std::unique_lock<std::mutex> lock(m);
        while ((mIsLoop) && (q.empty()))
        {
            // release lock as long as the wait and reaquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();

        return val;
    }

    void destory() {
        mIsLoop = false;
        c.notify_one();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(m);
        std::queue<T> emptyQueue;
        std::swap(q, emptyQueue);
    }
};

