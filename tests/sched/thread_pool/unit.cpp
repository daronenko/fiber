#include <fiber/sched/thread_pool.hpp>
#include <fiber/sched/task/submit.hpp>
#include <fiber/sync/wait_group.hpp>

#include <gtest/gtest.h>
#include <wheels/core/stop_watch.hpp>

#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;  // NOLINT
using namespace fiber;                 // NOLINT

class ThreadPoolTest : public ::testing::Test {};

TEST_F(ThreadPoolTest, WaitTask) {
  sched::ThreadPool pool{4};
  pool.Start();

  sync::WaitGroup wg;
  wg.Add(1);
  sched::task::Submit(pool, [&wg] {
    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, Wait) {
  sched::ThreadPool pool{4};
  pool.Start();

  sync::WaitGroup wg;
  wg.Add(1);
  sched::task::Submit(pool, [&wg] {
    std::this_thread::sleep_for(1s);
    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, MultiWait) {
  sched::ThreadPool pool{1};
  pool.Start();

  for (size_t i = 0; i < 3; ++i) {
    sync::WaitGroup wg;
    wg.Add(1);
    sched::task::Submit(pool, [&wg] {
      std::this_thread::sleep_for(1s);
      wg.Done();
    });

    wg.Wait();
  }

  pool.Stop();
}

TEST_F(ThreadPoolTest, ManyTasks) {
  sched::ThreadPool pool{4};
  pool.Start();

  static const size_t kTasks = 17;
  sync::WaitGroup wg;

  for (size_t i = 0; i < kTasks; ++i) {
    wg.Add(1);
    sched::task::Submit(pool, [&wg] {
      wg.Done();
    });
  }

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, Parallel) {
  sched::ThreadPool pool{4};
  pool.Start();

  std::atomic<bool> fast{false};
  sync::WaitGroup wg;

  wg.Add(1);
  sched::task::Submit(pool, [&] {
    std::this_thread::sleep_for(1s);
    wg.Done();
  });

  wg.Add(1);
  sched::task::Submit(pool, [&] {
    fast.store(true);
    wg.Done();
  });

  std::this_thread::sleep_for(100ms);
  ASSERT_TRUE(fast.load());

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, TwoPools) {
  sched::ThreadPool pool1{1};
  sched::ThreadPool pool2{1};

  pool1.Start();
  pool2.Start();

  wheels::StopWatch stop_watch;

  sync::WaitGroup wg1;
  wg1.Add(1);
  sched::task::Submit(pool1, [&] {
    std::this_thread::sleep_for(1s);
    wg1.Done();
  });

  sync::WaitGroup wg2;
  wg2.Add(1);
  sched::task::Submit(pool2, [&] {
    std::this_thread::sleep_for(1s);
    wg2.Done();
  });

  wg2.Wait();
  pool2.Stop();

  wg1.Wait();
  pool1.Stop();

  ASSERT_TRUE(stop_watch.Elapsed() < 1500ms);
}

TEST_F(ThreadPoolTest, Current) {
  sched::ThreadPool pool{1};
  pool.Start();

  ASSERT_EQ(sched::ThreadPool::Current(), nullptr);

  sync::WaitGroup wg;
  wg.Add(1);

  sched::task::Submit(pool, [&] {
    ASSERT_EQ(sched::ThreadPool::Current(), &pool);
    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, SubmitAfterWait) {
  sched::ThreadPool pool{4};
  pool.Start();

  sync::WaitGroup wg;

  wg.Add(1);
  sched::task::Submit(pool, [&] {
    std::this_thread::sleep_for(500ms);

    wg.Add(1);
    sched::task::Submit(pool, [&] {
      std::this_thread::sleep_for(500ms);
      wg.Done();
    });

    wg.Done();
  });

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, UseThreads) {
  sched::ThreadPool pool{4};
  pool.Start();

  sync::WaitGroup wg;

  for (size_t i = 0; i < 4; ++i) {
    wg.Add(1);
    sched::task::Submit(pool, [&wg] {
      std::this_thread::sleep_for(750ms);
      wg.Done();
    });
  }

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, TooManyThreads) {
  sched::ThreadPool pool{3};
  pool.Start();

  sync::WaitGroup wg;

  for (size_t i = 0; i < 4; ++i) {
    wg.Add(1);
    sched::task::Submit(pool, [&wg] {
      std::this_thread::sleep_for(750ms);
      wg.Done();
    });
  }

  wheels::StopWatch stop_watch;
  wg.Wait();
  pool.Stop();

  ASSERT_TRUE(stop_watch.Elapsed() > 1s);
}

TEST_F(ThreadPoolTest, TaskLifetime) {
  sched::ThreadPool pool{4};
  pool.Start();

  struct Widget {};
  auto w = std::make_shared<Widget>();

  sync::WaitGroup wg;
  for (int i = 0; i < 4; ++i) {
    wg.Add(1);
    sched::task::Submit(pool, [w, &wg] {
      wg.Done();
    });
  }

  std::this_thread::sleep_for(500ms);
  ASSERT_EQ(w.use_count(), 1);

  wg.Wait();
  pool.Stop();
}

TEST_F(ThreadPoolTest, Racy) {
  sched::ThreadPool pool{4};
  pool.Start();

  std::atomic<size_t> racy_counter{0};
  static const size_t kTasks = 100500;

  sync::WaitGroup wg;

  for (size_t i = 0; i < kTasks; ++i) {
    wg.Add(1);
    sched::task::Submit(pool, [&] {
      int old = racy_counter.load();
      racy_counter.store(old + 1);
      wg.Done();
    });
  }

  wg.Wait();
  pool.Stop();

  std::cout << "Racy counter value: " << racy_counter.load() << std::endl;
  ASSERT_LE(racy_counter.load(), kTasks);
}
