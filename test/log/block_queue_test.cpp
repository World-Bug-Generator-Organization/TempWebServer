#include "log/block_queue.h"

#include <gtest/gtest.h>

#include <istream>
#include <string>

namespace tempserver {

TEST(BlockQueueTest, Push) {
  BlockQueue<std::string> queue;
  queue.Push("test queue");
  std::string str;
  queue.Pop(str);
  EXPECT_EQ(str, "test queue");
}

TEST(BlockQueueTest, Pop) {
  BlockQueue<std::string> queue;
  queue.Push("test queue");
  std::string str;
  queue.Pop(str);
  EXPECT_EQ(str, "test queue");
}

}  // namespace tempserver