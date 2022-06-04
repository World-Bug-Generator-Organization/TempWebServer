#include <string>
#include <istream>
#include "log/block_queue.h"
#include "gtest/gtest.h"

namespace tempserver {
Test(BlockQueueTest, SimpleTest) {
  BlockQueue<std::string> queue;
  queue.Push("test queue");
  std::string str;
  queue.Pop(str);
  std::cout << str << std::endl;
}

}  // namespace tempserver
