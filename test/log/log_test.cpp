#include <gtest/gtest.h>
#include <unistd.h>

#include "log/log.h"

namespace tempserver {

TEST(LogTest, SimpleTest) {
  int num = 12;
  LOG_DEBUG("test1:%d", num);
  sleep(1);
}

}  // namespace tempserver