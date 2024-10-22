#include <fiber/sched/thread_pool.hpp>
#include <fiber/core/sched/go.hpp>
#include <fiber/core/sched/yield.hpp>
#include <fiber/sync/wait_group.hpp>

#include <wheels/core/stop_watch.hpp>
#include <gtest/gtest.h>

#include <thread>
#include <chrono>

using namespace std::chrono_literals;  // NOLINT
using namespace fiber;                 // NOLINT

void ExpectScheduler(sched::ThreadPool& pool) {
  EXPECT_EQ(sched::ThreadPool::Current(), &pool);
}

class ThreadPoolFibersTest : public ::testing::Test {};

TEST_F(ThreadPoolFibersTest, Go) {
  sched::ThreadPool pool{3};
  pool.Start();

  sync::WaitGroup wg;
  wg.Add(1);

  Go(pool, [&]() {
    ExpectScheduler(pool);
    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, GoGroup) {
  sched::ThreadPool pool{4};
  pool.Start();

  sync::WaitGroup wg;
  const size_t kFibers = 7;

  for (size_t i = 0; i < kFibers; ++i) {
    wg.Add(1);
    Go(pool, [&] {
      ExpectScheduler(pool);
      wg.Done();
    });
  }

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, GoChild) {
  sched::ThreadPool pool{3};
  pool.Start();

  sync::WaitGroup wg;
  wg.Add(1);

  Go(pool, [&] {
    ExpectScheduler(pool);
    wg.Add(1);
    Go([&] {
      ExpectScheduler(pool);
      wg.Done();
    });
    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, Parallel) {
  const size_t kThreads = 4;
  sched::ThreadPool pool{kThreads};
  pool.Start();

  wheels::StopWatch stop_watch;
  sync::WaitGroup wg;

  for (size_t i = 0; i < kThreads; ++i) {
    wg.Add(1);
    Go(pool, [&wg] {
      std::this_thread::sleep_for(1s);
      wg.Done();
    });
  }

  wg.Wait();
  EXPECT_TRUE(stop_watch.Elapsed() < 2s);
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, BaseYield) {
  sched::ThreadPool pool{1};
  pool.Start();

  sync::WaitGroup wg;
  wg.Add(1);

  Go(pool, [&] {
    Yield();
    ExpectScheduler(pool);
    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, YieldChild) {
  sched::ThreadPool pool{1};
  pool.Start();

  sync::WaitGroup wg;
  wg.Add(1);

  Go(pool, [&wg] {
    bool child = false;

    Go([&] {
      child = true;
    });

    while (!child) {
      Yield();
    }

    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, ForYield) {
  sched::ThreadPool pool{1};
  pool.Start();

  const size_t kYields = 128;
  size_t yields = 0;

  sync::WaitGroup wg;
  wg.Add(1);

  Go(pool, [&] {
    for (size_t i = 0; i < kYields; ++i) {
      Yield();
      ++yields;
    }
    wg.Done();
  });

  wg.Wait();
  EXPECT_EQ(yields, kYields);
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, PingPong) {
  sched::ThreadPool pool{1};
  pool.Start();

  bool start = false;
  int turn = 0;

  sync::WaitGroup wg;
  wg.Add(2);

  const size_t kRounds = 3;

  Go(pool, [&] {
    while (!start) {
      Yield();
    }

    for (size_t i = 0; i < kRounds; ++i) {
      EXPECT_EQ(turn, 0);
      turn ^= 1;
      Yield();
    }
    wg.Done();
  });

  Go(pool, [&] {
    start = true;
    Yield();

    for (size_t j = 0; j < kRounds; ++j) {
      EXPECT_EQ(turn, 1);
      turn ^= 1;
      Yield();
    }
    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, YieldGroup) {
  sched::ThreadPool pool{4};
  pool.Start();

  const size_t kFibers = 5;
  const size_t kYields = 7;

  sync::WaitGroup wg;

  for (size_t i = 0; i < kFibers; ++i) {
    wg.Add(1);
    Go(pool, [&] {
      for (size_t j = 0; j < kYields; ++j) {
        Yield();
      }
      wg.Done();
    });
  }

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolFibersTest, TwoPools) {
  sched::ThreadPool pool1{3};
  pool1.Start();

  sched::ThreadPool pool2{3};
  pool2.Start();

  sync::WaitGroup wg;
  wg.Add(2);

  Go(pool1, [&] {
    for (size_t i = 0; i < 2; ++i) {
      Yield();
      ExpectScheduler(pool1);
    }
    wg.Done();
  });

  Go(pool2, [&] {
    for (size_t j = 0; j < 3; ++j) {
      Yield();
      ExpectScheduler(pool2);
    }
    wg.Done();
  });

  wg.Wait();
  pool1.Stop();
  pool2.Stop();
}
