#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <iostream>
#include <future>


class ThreadPool {
public:
    ThreadPool(int numThreads);
    ~ThreadPool();

    template <class F, class... Args>
    void enqueue(F&& f, Args&&... args);
    void stopAndJoin();
    void killall(); 
    bool is_empty(); 

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;

};

inline ThreadPool::ThreadPool(int numThreads) : stop(false) {
    //std::cout << "Running parallel program with " << numThreads << " threads" << std::endl; 
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(
            [this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            }
        );
    }
}

inline void ThreadPool::killall(){
    std::unique_lock<std::mutex> lock(queueMutex);
    stop = true;
    if(!tasks.empty()){
        std::queue<std::function<void()>> empty; 
        std::swap(tasks, empty); 
        //std::cout << "Flushed the queue" << std::endl; 
    }

}

inline bool ThreadPool::is_empty(){
    std::unique_lock<std::mutex> lock(queueMutex);
    return tasks.empty(); 
}


inline void ThreadPool::stopAndJoin() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (auto& thread : threads)
        thread.join();
}

template <class F, class... Args>
void ThreadPool::enqueue(F&& f, Args&&... args) {
    { 
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace([f, args...]() { f(args...); });
    }
    condition.notify_all();
}

inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (auto& thread : threads)
        thread.join();
}
#endif