#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
namespace tempserver {

class ThreadPool {
   public:
    // 禁止隐式调用构造函数
    explicit ThreadPool(size_t threadCount = 8);

    ThreadPool() = default;

    // 移动构造函数
    ThreadPool(ThreadPool&&) = default;

    ~ThreadPool();

    template <class F>
    void AddTask(F&& task) {
        {
            // lock_guard与unique_lock类似
            // unique_lock比lock_guard灵活,但代价更大(网上是这么说的...回头再看)
            std::lock_guard<std::mutex> locker(pool_->mtx);
            // 将右值task原封不动转发到function<<void()>>的构造函数中
            pool_->tasks.emplace(std::forward<F>(task));
        }
        pool_->cond.notify_one();
    }

   private:
    struct Pool {
        std::mutex mtx;
        std::condition_variable cond;
        bool isClosed;
        std::queue<std::function<void()>> tasks;
    };
    std::shared_ptr<Pool> pool_;
};

}  // namespace tempserver