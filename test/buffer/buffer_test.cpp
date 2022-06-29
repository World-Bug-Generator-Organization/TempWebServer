#include "buffer/buffer.h"

#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <fcntl.h>

namespace tempserver {



TEST(Buffer, RetrieveAllToStr) {
    Buffer buffer;
    std::string input = "hello world";
    buffer.Append(input);

    std::cout<< buffer.RetrieveAllToStr() << std::endl;

    int fd = open("../LICENSE", O_RDONLY);
    int error=0;

    buffer.ReadFd(fd, &error);
    std::cout<< buffer.RetrieveAllToStr() << std::endl;

}

}  // namespace tempserver