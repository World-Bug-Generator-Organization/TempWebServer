#include <iostream>
#include <string>
#include <gtest/gtest.h>

#include "connection_pool/connection_pool.h"
namespace tempserver {

TEST(ConnectionPoolTest, SimpleTest) {
  auto instance = ConnectionPool::GetInstance();
  std::string url = "localhost";
  std::string user = "root";
  std::string password = "123456";
  std::string database_name = "yourdb";
  int port = 3306;
  int max_conn = 10;
  instance->Init(url, user, password, database_name, port, max_conn);
  MYSQL *test_sql;
  ConnectionPoolRAII conn_pool_raii(&test_sql, instance);
  std::string sql = "select * from user;";
  int re = mysql_query(test_sql, sql.c_str());
  if (re != 0) {
    std::cout << "mysql_query failed!" << mysql_error(test_sql) << std::endl;
  } else {
    MYSQL_RES *res = mysql_store_result(test_sql);
    // 获取列数
    int j = mysql_num_fields(res);

    //存储字段信息
    char *str_field[32];

    //获取字段名
    for (int i = 0; i < j; i++) {
      str_field[i] = mysql_fetch_field(res)->name;
    }

    //打印字段
    for (int i = 0; i < j; i++) printf("%10s\t", str_field[i]);
    printf("\n");

    //打印查询结果
    // MYSQL_ROW mysql_fetch_row(MYSQL_RES *result)
    // Fetches the next row from the result set
    while (auto column = mysql_fetch_row(res)) {
      printf("%10s\t%10s\n", column[0], column[1]);
    }
  }
}

}  // namespace tempserver