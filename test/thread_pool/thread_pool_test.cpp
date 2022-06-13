#include "thread_pool/thread_pool.h"

#include <gtest/gtest.h>
#include <string>
#include <iostream>

namespace tempserver {

void print(){
    std::cout << "hello world" << std::endl;
}

TEST(ThreadPool, Create) {
    ThreadPool threadpool(8);
    for(int i = 0; i < 18; i++) {
        threadpool.AddTask(print);
    }
    sleep(5);
}

}  // namespace tempserver