#include <fiber/sync/wait_group.hpp>

#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

using fiber::sync::WaitGroup;  // NOLINT

class WaitGroupTest : public ::testing::Test {};

TEST_F(WaitGroupTest, JustWorks) {
  WaitGroup wg;

  wg.Add(1);
  wg.Done();
  wg.Wait();
}

TEST_F(WaitGroupTest, InitZero) {
  WaitGroup wg;
  wg.Wait();
}

TEST_F(WaitGroupTest, AddCount) {
  WaitGroup wg;

  wg.Add(7);

  for (size_t i = 0; i < 7; ++i) {
    wg.Done();
  }

  wg.Wait();
}

TEST_F(WaitGroupTest, Wait) {
  WaitGroup wg;
  bool ready = false;

  wg.Add(1);

  std::thread producer([&] {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ready = true;
    wg.Done();
  });

  wg.Wait();
  ASSERT_TRUE(ready);

  producer.join();
}

TEST_F(WaitGroupTest, MultiWait) {
  std::vector<std::thread> threads;

  WaitGroup wg;

  std::atomic<size_t> work{0};

  static const size_t kWorkers = 3;
  static const size_t kWaiters = 4;

  wg.Add(kWorkers);

  for (size_t i = 0; i < kWaiters; ++i) {
    threads.emplace_back([&] {
      wg.Wait();
      ASSERT_EQ(work.load(), kWorkers);
    });
  }

  for (size_t i = 1; i <= kWorkers; ++i) {
    threads.emplace_back([&, i] {
      std::this_thread::sleep_for(std::chrono::milliseconds(256 * i));
      ++work;
      wg.Done();
    });
  }

  for (auto&& t : threads) {
    t.join();
  }
}

TEST_F(WaitGroupTest, BlockingWait) {
  WaitGroup wg;

  static const size_t kWorkers = 3;

  std::vector<std::thread> workers;
  std::atomic<size_t> work = 0;

  wg.Add(kWorkers);

  for (size_t i = 0; i < kWorkers; ++i) {
    workers.emplace_back([&] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      ++work;
      wg.Done();
    });
  }

  wg.Wait();

  for (auto& t : workers) {
    t.join();
  }
}

TEST_F(WaitGroupTest, Cyclic) {
  WaitGroup wg;

  for (size_t i = 0; i < 4; ++i) {
    bool flag = false;

    wg.Add(1);

    std::thread worker([&] {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      flag = true;
      wg.Done();
    });

    wg.Wait();

    ASSERT_TRUE(flag);

    worker.join();
  }
}
