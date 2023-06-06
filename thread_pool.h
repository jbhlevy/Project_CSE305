#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <iostream>
#include <future>

int MAX_TASKS = 2; 

class ThreadPool {
public:
    ThreadPool(int numThreads, int maxQueueSize);
    ~ThreadPool();

    template <class F, class... Args>
    void enqueue(F&& f, Args&&... args);
    void stopAndJoin();

private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    int maxQueueSize;
};

inline ThreadPool::ThreadPool(int numThreads, int maxQueueSize) : stop(false), maxQueueSize(maxQueueSize) {
    std::cout << numThreads << std::endl; 
    for (int i = 0; i < numThreads; ++i) {
        //std::cout << "Hello from thread " << i << std::endl; 
        threads.emplace_back(
            [this, i]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this]() { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                        //condition.notify_all();
                    }
                    std::cout << "Hello from thread " << i <<  " was assigned task at " << &task << std::endl; 
                    //std::cout << "Before tasking " << tasks.size() << std::endl; 
                    task();
                    std::cout << "Thread " << i << " Task "<< &task << " finished" << std::endl; 
                }
            }
        );
    }
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
        //std::cout << "t_before size " << tasks.size() << std::endl;
        std::unique_lock<std::mutex> lock(queueMutex);
        //condition.wait(lock, [this]() { return tasks.size() < maxQueueSize; });
        tasks.emplace([f, args...]() { f(args...); });
    }
    condition.notify_all();
    //std::cout << "t size " << tasks.size() << std::endl;
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

/*
// Example usage
void processData(int data) {
    std::cout << "Processing data: " << data << std::endl;
    // Perform some data processing
}

int main() {
    ThreadPool threadPool(4, 10); // Create a thread pool with 4 threads and maximum queue size of 10

    // Enqueue tasks to be executed by the thread pool
    for (int i = 0; i < 20; ++i) {
        threadPool.enqueue(processData, i);
    }

    // Wait for all tasks to complete
    threadPool.stopAndJoin();

    return 0;
}
*/
#endif