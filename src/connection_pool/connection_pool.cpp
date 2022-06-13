#include "connection_pool.h"
#include "log/log.h"
namespace tempserver {
void ConnectionPool::Init(std::string url, std::string user, std::string password, std::string database_name, int port, int max_conn) {
  std::unique_lock<std::mutex> lock(latch_);
  conns_.reserve(max_conn);
  MYSQL* conn = nullptr;
  for (int i = 0; i < max_conn; i++) {
    conn = mysql_init(conn);
    if (conn == nullptr) {
      LOG_ERROR("connect pool mysql_init error at %d time", i);
      exit(1);
    }
    conn = mysql_real_connect(conn, url.c_str(), user.c_str(), password.c_str(), database_name.c_str(), port, NULL, 0);
    if (conn == nullptr) {
      LOG_ERROR("connect pool mysql_real_connect error at %d time", i);
      exit(1);
    }
    conns_.emplace_back(conn);
  }
}
MYSQL* ConnectionPool::GetConnection() {
  std::unique_lock<std::mutex> lock(latch_);
  while (conns_.empty()) {  // 等待连接池资源
    cv_.wait(lock);
  }
  MYSQL* conn = conns_.back();
  conns_.pop_back();
  return conn;
}
bool ConnectionPool::ReleaseConnection(MYSQL* conn) {
  if (conn == nullptr) {
    return false;
  }
  std::unique_lock<std::mutex> lock(latch_);
  conns_.emplace_back(conn);
  cv_.notify_one();
  return true;
}
void ConnectionPool::DestroyAllPool() {
  std::unique_lock<std::mutex> lock(latch_);
  for (auto& conn : conns_) {
    mysql_close(conn);
  }
  conns_.clear();
}
ConnectionPool::~ConnectionPool() { DestroyAllPool(); }
ConnectionPool* ConnectionPool::GetInstance() {
  static ConnectionPool conn_pool;
  return &conn_pool;
}
ConnectionPoolRAII::ConnectionPoolRAII(MYSQL** conn, ConnectionPool* conn_pool) {
  *conn = conn_pool->GetConnection();
  conn_ = *conn;
  conn_pool_ = conn_pool;
}
ConnectionPoolRAII::~ConnectionPoolRAII() { conn_pool_->ReleaseConnection(conn_); }
}  // namespace tempserver