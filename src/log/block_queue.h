#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

namespace tempserver {

template <typename T>
class BlockQueue {
 public:
  explicit BlockQueue(size_t cap = 1000);
  bool Push(const T &item);
  bool Pop(T &item);
  bool Pop(T &item, int ms_timeout);

 private:
  bool IsFull();
  bool IsEmpty();

  std::mutex latch_;
  std::condition_variable producer_;
  std::condition_variable consumer_;
  std::queue<T> queue_;
  size_t capacity_;
};
}  // namespace tempserver