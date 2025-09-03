#pragma once
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class BoundedQueue {

public:
    explicit BoundedQueue(size_t cap) : capacity_(cap ? cap : 1){}

    bool push(T val) {
        std::unique_lock<std::mutex> lock(mtx_);
        not_full_.wait(lock, [&] {return closed_ || queue_.size() < capacity_;});
        
        if (closed_)
            return false;
            
        queue_.push(std::move(val));
        not_empty_.notify_one();
        return true;
    }

    bool pop(T& out) {
        std::unique_lock<std::mutex> lock(mtx_);
        not_empty_.wait(lock, [&] {return closed_ || !queue_.empty();});
        
        if (queue_.empty())
            return false;
        
        out = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
        return true;
    }

    void close() {
        std::lock_guard lock(mtx_);
        closed_ = true;
        not_empty_.notify_all();
        not_full_.notify_all();
    }
private:
    size_t capacity_;
    std::mutex mtx_;
    std::condition_variable not_full_, not_empty_;
    std::queue<T> queue_;
    bool closed_ = false;
};