#pragma once
#include <deque>
#include <future>

struct thread_pool {
  std::mutex mutex;
  std::deque<std::packaged_task<void()>> tasks;
  std::condition_variable condition;
  std::vector<std::future<void>> threads;

  void start(size_t size = 1)
  {
    for (size_t i = 0; i < size; ++i)
    {
      threads.push_back(
        std::async(
          std::launch::async,
          [this]{ thread_task(); }
        )
      );
    }
  }

  template<class F, class R=std::result_of_t<F&()>>
  std::future<R> queue(F&& func_callback) {
    std::packaged_task<R()> task(std::forward<F>(func_callback));
    auto result_future = task.get_future();
    {
      std::unique_lock<std::mutex> lock(mutex);
      tasks.emplace_back(std::move(task));
    }
    condition.notify_one();
    return result_future;
  }

  void stop() {
    {
      std::unique_lock<std::mutex> lock(mutex);
      for(auto&& thread : threads){
        tasks.push_back({});
      }
    }
    condition.notify_all();
    threads.clear();
  }

  ~thread_pool() {
    stop();
  }

  private:
    void thread_task()
    {
      while(true){
        std::packaged_task<void()> func_callback;
        {
          std::unique_lock<std::mutex> lock(mutex);
          if (tasks.empty()){
            condition.wait(lock, [&]{
              return !tasks.empty();
            });
          }
          func_callback = std::move(tasks.front());
          tasks.pop_front();
        }
        if (!func_callback.valid()) 
          return;

        func_callback();
      }
    }
};