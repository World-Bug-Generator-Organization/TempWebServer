#pragma once

#include <chrono>
#include <functional>
#include <queue>
#include <unordered_map>
#include <vector>

using TimeoutCallBack = std::function<void()>;
using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::milliseconds;
using TimeStamp = Clock::time_point;

namespace tempserver {

struct TimerNode {
    int id;
    TimeStamp expires;
    TimeoutCallBack cb;
    bool operator<(const TimerNode& t) { return expires < t.expires; }
};

class HeapTimer {
   private:
    // 小顶堆
    std::vector<TimerNode> heap_;
    // timer id到下标的映射
    std::unordered_map<int, size_t> ref_;

    // 删除指定id的定时器
    void Del(size_t i);
    void SwapNode(size_t i, size_t j);
    // 将节点向上调整
    void SiftUp(size_t i);
    // 将节点向下调整
    bool SiftDown(size_t i, size_t n);

   public:
    HeapTimer() {
        // 预留一部分空间
        heap_.reserve(64);
    }

    ~HeapTimer() { Clear(); }

    void Clear();

    void Adjust(int id, int timeout);

    void Add(int id, int time_out, const TimeoutCallBack& cb);

    void DoWork(int id);

    void Tick();

    void Pop();

    int GetNextTick();
};

}  // namespace tempserver
