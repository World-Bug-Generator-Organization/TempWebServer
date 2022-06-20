#include "timer/heap_timer.h"

#include <gtest/gtest.h>

#include <functional>
#include <iostream>
#include <string>

namespace tempserver {

void print(int time_out) { std::cout << "Time out: " << time_out << std::endl; }

TEST(Timer, Add) {
    auto heap_timer = HeapTimer();
    for (int time_out = 1; time_out < 4; time_out++) {
        heap_timer.Add(time_out, time_out, [time_out]() { print(time_out); });
    }
    while (heap_timer.GetNextTick() != -1) {
        sleep(1);
    }
}

TEST(Timer, Clear) {
    auto heap_timer = HeapTimer();
    for (int time_out = 3; time_out < 5; time_out++) {
        heap_timer.Add(time_out, time_out, [time_out]() { print(time_out); });
    }
    while (heap_timer.GetNextTick() != -1) {
        sleep(1);
    }
    // 上面会打印3~4
    // 下面不会打印7~8

    for (int time_out = 7; time_out < 9; time_out++) {
        heap_timer.Add(time_out, time_out, [time_out]() { print(time_out); });
    }
    heap_timer.Clear();
    while (heap_timer.GetNextTick() != -1) {
        sleep(1);
    }
}

TEST(Timer, ReAdd) {
    auto heap_timer = HeapTimer();

    int id = 3;
    int time_out = 3;
    heap_timer.Add(id, time_out, [time_out]() { print(time_out); });

    // 修改time_out为5
    time_out = 5;
    heap_timer.Add(3, time_out, [time_out]() { print(time_out); });

    while (heap_timer.GetNextTick() != -1) {
        sleep(1);
    }
}

}  // namespace tempserver