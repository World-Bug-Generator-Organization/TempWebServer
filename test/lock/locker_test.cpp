#include "lock/locker.h"

#include <gtest/gtest.h>

#include <istream>
#include <string>

namespace tempserver {

TEST(Locker, Lock) {
    Locker lock;
    lock.Lock();

    std::cout<< "test" << std::endl;
    
    lock.Unlock();
}

}  // namespace tempserver