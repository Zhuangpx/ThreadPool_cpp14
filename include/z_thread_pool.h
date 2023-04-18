#ifndef _Z_THREAD_POOL_H
#define _Z_THREAD_POOL_H


#include <mutex>
#include <functional>
#include <future>
#include <thread>
#include <utility>
#include <queue>
#include <vector>

#include "z_safe_queue.h"

using wrapper_function = std::function<void()>;

class zThreadPool
{
public:
  zThreadPool(const int _threads_size = 4)
    : threads_(std::vector<std::thread>(_threads_size))
  {
    for (int i = 0; i < threads_.size(); ++i)
    {
      threads_.at(i) = std::thread(zThreadWorker(this, i));
    }
  }
  zThreadPool(const zThreadPool &) = delete;
  zThreadPool(zThreadPool &&) = delete;
  zThreadPool &operator=(const zThreadPool &) = delete;
  zThreadPool &operator=(zThreadPool &&) = delete;

  ~zThreadPool() { shut_down(); }

  template <typename _Func, typename... _Args>
  auto submit(_Func &&f, _Args &&...args) -> std::future<decltype(f(args...))>
  {
    std::function<decltype(f(args...))()> _func = std::bind(std::forward<_Func>(f), std::forward<_Args>(args)...);
    auto _task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(_func);

    wrapper_function _wrap_func = [_task_ptr]() { (*_task_ptr)(); };
    task_queue_.emplace(_wrap_func);
    return _task_ptr->get_future();
  }

private:

  class zThreadWorker
  {
    public:
      zThreadWorker() = default;
      zThreadWorker(zThreadPool* _pool_ptr, const int _id)
        : pool_ptr_(_pool_ptr), id_(_id)
      {}
      void operator()()
      {
        wrapper_function _warp_func;
        bool _pop_res = false;
        while(true)
        {
          if(!(_pop_res = pool_ptr_->task_queue_.pop(_warp_func)))
          { break; }
          if(_pop_res)
          { _warp_func(); }
        }
      }
    private:
      int id_;
      zThreadPool* pool_ptr_;
  };

  //  线程池关闭 先把任务队列弃用 然后把未完成的join
  void shut_down()
  {
    task_queue_.close();
    for(int i=0; i<threads_.size(); i++)
    {
      if (threads_.at(i).joinable())
      {
        threads_.at(i).join();
      }
    }
  }

  zSafeQueue<wrapper_function> task_queue_;
  std::vector<std::thread> threads_;

};

#endif

