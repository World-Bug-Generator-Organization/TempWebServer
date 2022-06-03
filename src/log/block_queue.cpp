#include <string>
#include "block_queue.h"
namespace tempserver {

template <typename T>
BlockQueue<T>::BlockQueue(size_t cap) : capacity_(cap) {}

template <typename T>
bool BlockQueue<T>::IsFull() {
  return queue_.size() >= capacity_;
}

template <typename T>
bool BlockQueue<T>::IsEmpty() {
  return queue_.size() == 0;
}

template <typename T>
bool BlockQueue<T>::Push(const T &item) {
  std::unique_lock<std::mutex> lock(latch_);
  while (IsFull()) {
    cv_.wait(lock);
  }
  queue_.emplace(item);
  return true;
}

template <typename T>
bool BlockQueue<T>::Pop(T &item) {
  std::unique_lock<std::mutex> lock(latch_);
  while (IsEmpty()) {
    cv_.wait(lock);
  }
  queue_.emplace(item);
  return true;
}

template <typename T>
bool BlockQueue<T>::Pop(T &item, int ms_timeout) {
  std::unique_lock<std::mutex> lock(latch_);
  while (IsEmpty()) {
    auto status = cv_.wait_for(lock, std::chrono::seconds(ms_timeout));
    if (status == std::cv_status::timeout) {
      return false;
    }
  }
  queue_.emplace(item);
  return true;
}
/*
模板类在.h中定义，在.cpp中实现:https://zhuanlan.zhihu.com/p/147623943
*/
template class BlockQueue<std::string>;
}  // namespace tempserver