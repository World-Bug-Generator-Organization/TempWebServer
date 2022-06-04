#ifndef LOCKER_H
#define LOCKER_H

#include <pthread.h>
#include <semaphore.h>

#include <stdexcept>

#define PROCESS_LOCAL_SEMAPHORE 0

namespace tempserver {

// 封装信号量
class Semaphore {
   private:
    sem_t semaphore_;

   public:
    Semaphore() {
        if (sem_init(&semaphore_, PROCESS_LOCAL_SEMAPHORE, 0) != 0) {
            throw std::runtime_error("信号量初始化错误");
        }
    }
    Semaphore(int num) {
        if (sem_init(&semaphore_, PROCESS_LOCAL_SEMAPHORE, num) != 0) {
            throw std::runtime_error("信号量初始化错误");
        }
    }
    ~Semaphore() { sem_destroy(&semaphore_); }
    bool Wait() { return sem_wait(&semaphore_) == 0; }
    bool Post() { return sem_post(&semaphore_) == 0; }
};

// 封装互斥锁
class Locker {
   private:
    pthread_mutex_t mutex_;

   public:
    Locker() {
        // 使用默认参数,不检测重复加锁等问题(会产生不可预期后果)
        if (pthread_mutex_init(&mutex_, NULL) != 0) {
            throw std::runtime_error("互斥锁初始化错误");
        }
    }
    ~Locker() { pthread_mutex_destroy(&mutex_); }
    bool Lock() { return pthread_mutex_lock(&mutex_) == 0; }
    bool Unlock() { return pthread_mutex_unlock(&mutex_) == 0; }
    pthread_mutex_t *Get() { return &mutex_; }
};

// 封装条件变量
class Cond {
   private:
    pthread_cond_t cond_;

   public:
    Cond() {
        if (pthread_cond_init(&cond_, NULL) != 0) {
            throw std::runtime_error("条件变量初始化错误");
        }
    };
    ~Cond() { pthread_cond_destroy(&cond_); }
    bool Wait(pthread_mutex_t *mutex) {
        // 需要在使用前将mutex加锁,成功调用后依然是加锁状态

        // 将自己加入等待队列后加锁 mutex(允许其他线程操作)
        // 自身被唤醒后自动加锁 mutex(可能阻塞)
        // <<Linux高性能服务器编程>> 14.6节
        // pthread_mutex_lock(mutex);
        int ret = pthread_cond_wait(&cond_, mutex);
        // pthread_mutex_unlock(mutex);
        return ret == 0;
    }
    bool TimeWait(pthread_mutex_t *mutex, struct timespec *t) {
        // 需要在使用前将mutex加锁,成功调用后依然是加锁状态

        // 同 Wait
        // pthread_mutex_lock(mutex);
        int ret = pthread_cond_timedwait(&cond_, mutex, t);
        // pthread_mutex_unlock(mutex);
        return ret == 0;
    }
    bool Signal() { return pthread_cond_signal(&cond_) == 0; }
    bool Broadcast() { return pthread_cond_broadcast(&cond_) == 0; }
};

}  // namespace tempserver
#endif