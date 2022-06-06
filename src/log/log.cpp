#include <chrono>
#include <iomanip>
#include <map>
#include <unistd.h>
#include "log/log.h"

namespace tempserver {
std::string Log::GetDate() {
  auto now = std::chrono::system_clock::now();
  auto timet = std::chrono::system_clock::to_time_t(now);
  auto local_time = *std::gmtime(&timet);
  std::stringstream ss;
  std::string date;
  ss << std::put_time(&local_time, "%Y_%m_%d");
  ss >> date;
  return date;
}
bool Log::UpdateFileHandle() {
  std::string date = GetDate();
  if (log_file_.is_open()) {
    if (cur_date_ != date) {
      log_file_.close();
      log_file_.open(date + ".log", std::ios::out | std::ios::app);
    }
  } else {
    log_file_.open(date + ".log", std::ios::out | std::ios::app);
  }

  cur_date_ = date;
  if (!log_file_.is_open()) {
    return false;
  }
  return true;
}
Log::Log(size_t queue_cap) : queue_(queue_cap) {
  thread_live_ = true;
  auto task = [&] {
    while (thread_live_) {
      UpdateFileHandle();
      std::string info;
      queue_.Pop(info);
      log_file_ << info << std::endl;
    }
  };
  write_thread_ = std::thread(task);
}

Log::~Log() {
  sleep(1);  // 等待日志写入完成
  thread_live_ = false;
  queue_.Close();
  write_thread_.join();
}

Log* Log::GetInstance() {
  static Log log;
  return &log;
}

}  // namespace tempserver