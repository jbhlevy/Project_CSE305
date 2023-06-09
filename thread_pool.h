#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
class ThreadPool {
public:
    ThreadPool(int numThreads);
    ~ThreadPool();

    template <class F, class... Args>
    void enqueue(F&& f, Args&&... args);

    void stopAndJoin();
    void killall();
    bool is_empty();
    bool isFull();

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable notEmptyCondition;
    std::condition_variable notFullCondition;
    bool stop;
    int maxQueueSize;
};

inline ThreadPool::ThreadPool(int numThreads) : stop(false) {
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([this]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    notEmptyCondition.wait(lock, [this]() { return stop || !tasks.empty(); });
                    if (stop && tasks.empty())
                        return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                notFullCondition.notify_one();
                task();
            }
        });
    }
}

inline void ThreadPool::killall() {
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
    std::queue<std::function<void()>> emptyQueue;
    std::swap(tasks, emptyQueue);
}

inline bool ThreadPool::is_empty() {
    std::unique_lock<std::mutex> lock(queueMutex);
    return tasks.empty();
}

inline bool ThreadPool::isFull() {
    std::unique_lock<std::mutex> lock(queueMutex);
    return tasks.size() >= maxQueueSize;
}

inline void ThreadPool::stopAndJoin() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
        //notEmptyCondition.wait(lock, [this]() { return tasks.empty(); });
    }
    //notEmptyCondition.notify_all();
    for (auto& thread : threads)
        thread.join();
}


template <class F, class... Args>
void ThreadPool::enqueue(F&& f, Args&&... args) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace([f, args...]() { f(args...); });
    }
    notEmptyCondition.notify_one();
}

inline ThreadPool::~ThreadPool() {
    stopAndJoin();
}

#endif  // THREAD_POOL_H