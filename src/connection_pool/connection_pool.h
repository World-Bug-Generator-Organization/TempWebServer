#pragma once
#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>
#include <mysql/mysql.h>
namespace tempserver {

class ConnectionPool {
 public:
  static ConnectionPool* GetInstance();                                                                                   // 获取单一示例
  void Init(std::string url, std::string user, std::string password, std::string database_name, int port, int max_conn);  // 连接处初始化
  MYSQL* GetConnection();                                                                                                 // 获取数据库链接
  bool ReleaseConnection(MYSQL* conn);                                                                                    // 释放数据库链接
  void DestroyAllPool();                                                                                                  // 销毁连接池

 private:
  ConnectionPool() = default;  // 默认构造函数
  ~ConnectionPool();

  std::mutex latch_;
  std::condition_variable cv_;
  std::vector<MYSQL*> conns_;
};

class ConnectionPoolRAII {  // RAII:Resource Acquisition Is Initialization
 public:
  ConnectionPoolRAII(MYSQL** conn, ConnectionPool* conn_pool);
  ~ConnectionPoolRAII();

 private:
  MYSQL* conn_;
  ConnectionPool* conn_pool_;
};

}  // namespace tempserver