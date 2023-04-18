<!--
 * @Autor: violet apricity ( Zhuangpx )
 * @Date: 2023-04-18 01:09:28
 * @LastEditors: violet apricity ( Zhuangpx )
 * @LastEditTime: 2023-04-18 12:24:16
 * @FilePath: \ThreadPool_cpp14\README.md
 * @Description:  Zhuangpx : Violet && Apricity:/ The warmth of the sun in the winter /
-->
# ThreadPool_cpp14

A simple Thread Pool implementation base on C++14.

## How do to

### Safe Queue

Use `std::mutex` and `std::condition_variable` , make the Queue thread-safe.
And then use this Safe Queue for the Thread Pool's task queue.
In addition, a **close** operation is required to accompany the shut down of the thread pool.

### Thread Pool

Three important elements here:

- **Tasks Queue**. This is where the work that has to be done is stored. Anyway, it must be thread-safe.
- **Thread Pool**. This is set of threads (or workers) that continuously take work from the queue and do it.
- **Completed Tasks**. When the Thread has finished the work we return "something" to notify that the work has finished.

#### task submit

```c++
using wrapper_function = std::function<void()>;
//  模板函数 + 可变参数
//  尾返回类型: std::future访问异步操作结果 + decltype自动类型推导 + (参数)通用引用
template <typename _Func, typename... _Args>
auto submit(_Func &&f, _Args &&...args) -> std::future<decltype(f(args...))>
{
  //  std::function: std::bind绑定绑定函数和参数 + std::forward完美转发
  std::function<decltype(f(args...))()> _func = std::bind(std::forward<_Func>(f), std::forward<_Args>(args)...);
  //  std::make_shared智能指针 + std::packaged_task封装异步调用对象
  auto _task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(_func);
  //  类似装饰器的装饰函数 std::function<void()>之后在函数体里调用
  wrapper_function _wrap_func = [_task_ptr]() { (*_task_ptr)(); };
  //  入队
  task_queue_.emplace(_wrap_func);
  //  get_future拿到结果
  return _task_ptr->get_future();
}
```

#### task worker

Someting is need to be the real task worker. That's Thread Worker here.
A Thread Worker is a private member class, set up in the Thread Pool as the built-in thread worker class to do the real work.

With the pointer to a ThreadPool, override the `()` symbol to fetch the wrapper_function from the task queue and execute it.

```c++
using wrapper_function = std::function<void()>;
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
```

## 来点中文

- 基于C++14的简单线程池
- 使用C++标准库中的线程+锁+条件变量实现并发
- 使用C++14标准库提供的 `std::bind`，`std::forward`，`std::future`，`decltype` 等新特性。

## Useful link && Thans to && References

[@mtrebi thread-pool](https://github.com/mtrebi/thread-pool)

[现代C++教程：快速上手C++ 11/14/17/20](https://changkun.de/modern-cpp/zh-cn/07-thread/)

[知乎二创](https://zhuanlan.zhihu.com/p/367309864)
