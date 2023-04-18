#ifndef _Z_SAFE_QUEUE_H
#define _Z_SAFE_QUEUE_H

#include <mutex>
#include <queue>
// #include <functional>
#include <future> //  condition_variable future
// #include <thread>
// #include <utility>
// #include <vector>

template <typename _Tp>
class zSafeQueue
{
public:
  zSafeQueue() = default;
  zSafeQueue(zSafeQueue&& _cp_safe_queue) = delete;
  ~zSafeQueue() = default;

  bool empty()
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    return queue_.empty();
  }

  int size()
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    return queue_.size();
  }

  void push(const _Tp& _item)
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    queue_.push(_item);
    cond_notify_one();
  }
  void push(_Tp&& _item)
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    queue_.push(_item);
    cond_notify_one();
  }

  void emplace(const _Tp& _item)
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    queue_.emplace(_item);
    cond_notify_one();
  }
  void emplace(_Tp&& _item)
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    queue_.emplace(_item);
    cond_notify_one();
  }

  bool pop(_Tp& _item)
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    if(queue_.empty())
    { return false; }
    //  条件变量等待 非空 或者 队列弃用
    cond_.wait(_lock, [&](){ return !queue_.empty() || is_close_; } );
    //  空且弃用 false
    if(queue_.empty())
    { return false; }
    //  非空 true
    _item = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  void close()
  {
    std::unique_lock<std::mutex> _lock(mutex_);
    is_close_ = true;
    cond_notify_all();
  }

  void cond_notify_all()
  {
    cond_.notify_all();
  }
  void cond_notify_one()
  {
    cond_.notify_one();
  }

private:
  std::queue<_Tp> queue_;
  std::condition_variable cond_;
  std::mutex mutex_;
  bool is_close_ = false;  //  标记队列是否弃用
};


#endif

