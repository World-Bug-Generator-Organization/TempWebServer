#include "thread_pool.h"

#include <assert.h>

namespace tempserver {

ThreadPool::ThreadPool(size_t threadCount)
    : pool_(std::make_shared<Pool>()) {
    //线程池大小为正
    assert(threadCount > 0);
    for (size_t i = 0; i < threadCount; i++) {
        std::thread([pool = pool_] {
            std::unique_lock<std::mutex> locker(pool->mtx);
            while (true) {
                if (!pool->tasks.empty()) {
                    // 将task中的函数转为右值引用
                    // 会调用function<void()>的拷贝构造函数
                    auto task = std::move(pool->tasks.front());
                    pool->tasks.pop();
                    locker.unlock();
                    task();
                    locker.lock();
                } else if (pool->isClosed)
                    break;
                else
                    // 无任务,等待新任务
                    pool->cond.wait(locker);
            }
        }).detach();
        // 与主线程分离
    }
}

ThreadPool::~ThreadPool() {
    if (static_cast<bool>(pool_)) {
        {
            std::lock_guard<std::mutex> locker(pool_->mtx);
            pool_->isClosed = true;
        }
        pool_->cond.notify_all();
    }
}

}  // namespace tempserver
