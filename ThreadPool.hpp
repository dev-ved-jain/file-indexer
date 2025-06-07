#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>

class ThreadPool {
private:
    // n - worker threads
    std::vector<std::thread> workers;
    // task queue which will be used to push file processing jobs
    std::queue<std::function<void()>> tasks; 
    // queue mutex is needed to protect access to task queue
    std::mutex queueMutex;
    // condition variable will be use to notify worker thread when new task will arrive
    std::condition_variable condition;
    // tells thread to stop on shutdown
    std::atomic<bool> stop;

public:
    //constructor
    ThreadPool(size_t numThreads) : stop(false) {
        // create numThreads worker threads
        for(size_t i = 0 ; i < numThreads ; ++i) {
            workers.emplace_back([this]() {
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queueMutex);
                        this->condition.wait(lock, [this]() {
                            return this->stop || !this->tasks.empty();
                        });

                        if(this->stop && this->tasks.empty()) return;

                        // safely assign a task from tasks queue
                        task = std::move(this->tasks.front());

                        this->tasks.pop();
                    }
                    // execute the task
                    task();
                }   
            });
        }
    }

    // add a new task to queue
    template<class F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace(std::forward<F>(task));
        }
        condition.notify_one();
    }

    // destructor
    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();

        // wait for threads to finish before we destroy ThreadPool
        for(std::thread& worker : workers) {
            worker.join();
        }
    }
};