#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    ThreadPool(int numThreads);
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
};

inline ThreadPool::ThreadPool(int numThreads) : stop(false) {
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
    condition.notify_one();
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

#endif  // THREAD_POOL_H

// Example usage
/*
// A function to be executed by the thread pool
void processData(int data) {
    std::cout << "Processing data: " << data << std::endl;
    // Perform some data processing
}

int main() {
    ThreadPool threadPool(4); // Create a thread pool with 4 threads

    // Enqueue tasks to be executed by the thread pool
    for (int i = 0; i < 10; ++i) {
        threadPool.enqueue(processData, i);
    }

    // Wait for all tasks to complete
    // (Note: This is optional and depends on your specific use case)
    // ...

    return 0;
}
*/