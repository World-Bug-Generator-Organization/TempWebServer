#include "heap_timer.h"

#include <assert.h>

namespace tempserver {

void HeapTimer::SiftUp(size_t i) {
    // 尝试将指定位置元素向上转移
    assert(i >= 0 && i < heap_.size());
    // 父节点
    size_t j = (i - 1) / 2;

    while (j >= 0) {
        // 不用继续sift up
        if (heap_[j] < heap_[i]) break;
        SwapNode(i, j);
        i = j;
        j = (i - 1) / 2;
    }
}

bool HeapTimer::SiftDown(size_t i, size_t n) {
    // 尝试将指定位置元素向下转移
    assert(i >= 0 && i < heap_.size());
    assert(n >= 0 && n <= heap_.size());
    size_t index = i;
    // 左子节点
    size_t j = i * 2 + 1;

    while (j < n) {
        // 对比左子节点和右子节点谁更小
        if (j + 1 < n && heap_[j + 1] < heap_[j]) j++;
        if (heap_[i] < heap_[j]) break;
        SwapNode(i, j);
        i = j;
        j = i * 2 + 1;
    }

    // 向下移动任意位置返回True,否则返回False
    return i > index;
}

void HeapTimer::Del(size_t i) {
    // 堆非空且位置合法
    assert(!heap_.empty() && i >= 0 && i < heap_.size());

    // 将指定节点与堆尾元素互换,调整堆
    size_t n = heap_.size() - 1;
    assert(i <= n);
    if (i < n) {
        SwapNode(i, n);
        // 新元素无法向下转移的时候尝试向上转移
        if (!SiftDown(i, n)) {
            SiftUp(i);
        }
    }
    ref_.erase(heap_.back().id);
    heap_.pop_back();
}

void HeapTimer::SwapNode(size_t i, size_t j) {
    assert(i >= 0 && i < heap_.size());
    assert(j >= 0 && j < heap_.size());
    std::swap(heap_[i], heap_[j]);
    // 注意切换坐标映射
    ref_[heap_[i].id] = i;
    ref_[heap_[j].id] = j;
}

void HeapTimer::Clear() {
    ref_.clear();
    heap_.clear();
}

void HeapTimer::Adjust(int id, int timeout) {
    // 更改指定定时器的剩余时间
    assert(!heap_.empty() && ref_.count(id) != 0);
    heap_[ref_[id]].expires = Clock::now() + MS(timeout);

    if (!SiftDown(ref_[id], heap_.size())) {
        SiftUp(ref_[id]);
    }
}

void HeapTimer::Add(int id, int time_out, const TimeoutCallBack& cb) {
    assert(id >= 0);
    if (ref_.count(id) == 0) {
        // 新节点
        size_t i = heap_.size();
        ref_[id] = i;
        heap_.push_back({id, Clock::now() + MS(time_out), cb});
        SiftUp(i);
    } else {
        // 已有节点
        size_t i = ref_[id];
        heap_[i].expires = Clock::now() + MS(time_out);
        heap_[i].cb = cb;
        if (!SiftDown(i, heap_.size())) {
            SiftUp(i);
        }
    }
}

void HeapTimer::DoWork(int id) {
    if (heap_.empty() || ref_.count(id) == 0) {
        return;
    }
    size_t i = ref_[id];
    TimerNode node = heap_[i];
    node.cb();
    Del(i);
}

void HeapTimer::Tick() {
    if (heap_.empty()) {
        return;
    }
    while (!heap_.empty()) {
        TimerNode node = heap_.front();
        if (std::chrono::duration_cast<MS>(node.expires - Clock::now())
                .count() > 0) {
            break;
        }
        node.cb();
        Pop();
    }
}

void HeapTimer::Pop() {
    assert(!heap_.empty());
    Del(0);
}

int HeapTimer::GetNextTick() {
    Tick();
    size_t res = -1;
    if (!heap_.empty()) {
        res =
            std::chrono::duration_cast<MS>(heap_.front().expires - Clock::now())
                .count();
        if (res < 0) {
            res = 0;
        }
    }
    return res;
}

}  // namespace tempserver
