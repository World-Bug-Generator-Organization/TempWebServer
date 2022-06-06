#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <thread>

#include "log/block_queue.h"

const int kMaxInfoLen = 1000;
namespace tempserver {
enum class Level { kDebug, kInfo, kWarn, kError };

class Log {
 public:
  static Log* GetInstance();

  template <class... T>
  void write(Level level, const char* format, T&... args);

 private:
  Log(size_t queue_cap = 1000);
  ~Log();
  std::string GetDate();
  bool UpdateFileHandle();

  std::string cur_date_;
  std::ofstream log_file_;
  std::thread write_thread_;
  BlockQueue<std::string> queue_;
};

// 模板成员函数在头文件中定义
template <class... T>
void Log::write(Level level, const char* format, T&... args) {
  static std::map<Level, std::string> prefix{
      {Level::kDebug, "[Debug]:"}, {Level::kInfo, "[Info]:"}, {Level::kWarn, "[Warn]:"}, {Level::kError, "[Error]:"}};
  static char info_buffer_[kMaxInfoLen];
  int len = snprintf(info_buffer_, kMaxInfoLen, format, args...);
  std::string info(info_buffer_, info_buffer_ + len);
  queue_.Push(prefix[level] + info);
}

// 内联函数在头文件定义
template <class... T>
void inline LOG_DEBUG(const char* format, T&... args) {
  // Log::GetInstance()->write(Level::kDebug, format, args...);
  Log::GetInstance();
}
template <class... T>
void inline LOG_INFO(const char* format, T&... args) {
  Log::GetInstance()->write(Level::kInfo, format, args...);
}
template <class... T>
void inline LOG_WARN(const char* format, T&... args) {
  Log::GetInstance()->write(Level::kWarn, format, args...);
}
template <class... T>
void inline LOG_ERROR(const char* format, T&... args) {
  Log::GetInstance()->write(Level::kError, format, args...);
}
}  // namespace tempserver