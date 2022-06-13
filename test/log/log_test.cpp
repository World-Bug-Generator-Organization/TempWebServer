#include <gtest/gtest.h>


#include "log/log.h"

namespace tempserver {

TEST(LogTest, SimpleTest) {
  int num = 12;
  LOG_DEBUG("test1:%d", num);
  LOG_DEBUG("test2:%d", num);
  LOG_DEBUG("test3:%d", num);
}

}  // namespace tempserver