#include <string>
#include <iostream>
#include "block_queue.h"
namespace tempserver {

template <typename T>
BlockQueue<T>::BlockQueue(size_t cap) : capacity_(cap), is_close_(false) {}

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
    producer_.wait(lock);
    if (is_close_) {
      return false;
    }
  }
  queue_.emplace(item);
  consumer_.notify_one();
  return true;
}

template <typename T>
bool BlockQueue<T>::Pop(T &item) {
  std::unique_lock<std::mutex> lock(latch_);
  while (IsEmpty()) {
    consumer_.wait(lock);
    if (is_close_) {
      return false;
    }
  }
  item = queue_.front();
  queue_.pop();
  producer_.notify_one();
  return true;
}

template <typename T>
bool BlockQueue<T>::Pop(T &item, int ms_timeout) {
  std::unique_lock<std::mutex> lock(latch_);
  while (IsEmpty()) {
    auto status = consumer_.wait_for(lock, std::chrono::seconds(ms_timeout));
    if (status == std::cv_status::timeout) {
      return false;
    }
    if (is_close_) {
      return false;
    }
  }
  item = queue_.front();
  queue_.pop();
  producer_.notify_one();
  return true;
}

template <typename T>
void BlockQueue<T>::Close() {
  is_close_ = true;
  producer_.notify_all();
  consumer_.notify_all();
}
/*
模板类在.h中定义，在.cpp中实现:https://zhuanlan.zhihu.com/p/147623943
*/
template class BlockQueue<std::string>;
}  // namespace tempserver