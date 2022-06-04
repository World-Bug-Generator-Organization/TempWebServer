#include "log/block_queue.h"

#include <gtest/gtest.h>

#include <istream>
#include <string>

namespace tempserver {

TEST(BlockQueue, Pop) {
    BlockQueue<std::string> queue;
    queue.Push("test queue");
    std::string str;
    queue.Pop(str);
    std::cout << str << std::endl;

    // std::cout << "test" << std::endl;
}

}  // namespace tempserver