#include <fiber/sched/thread_pool.hpp>
#include <fiber/sync/wait_group.hpp>

#include <gtest/gtest.h>
#include <fmt/core.h>

using namespace fiber;  // NOLINT

class ThreadPoolTest : public ::testing::Test {};

TEST_F(ThreadPoolTest, JustWorksIntrusive) {
  sched::ThreadPool pool{4};
  pool.Start();

  class DoWork : public sched::task::TaskBase {
   public:
    explicit DoWork(sync::WaitGroup& wg)
        : wg_(wg) {
    }

    void Run() noexcept override {
      wg_.Done();
    }

   private:
    sync::WaitGroup& wg_;
  };

  {
    sync::WaitGroup wg;

    DoWork work{wg};

    wg.Add(1);
    pool.Submit(&work);
    wg.Wait();
  }

  pool.Stop();
}
