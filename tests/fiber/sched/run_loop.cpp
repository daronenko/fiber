#include <fiber/sched/run_loop.hpp>
#include <fiber/core/sched/go.hpp>
#include <fiber/core/sched/yield.hpp>

#include <gtest/gtest.h>

using namespace fiber;  // NOLINT

class RunLoopFibersTest : public ::testing::Test {};

TEST_F(RunLoopFibersTest, Go) {
  sched::RunLoop loop;

  Go(loop, [] {});

  size_t tasks = loop.Run();
  EXPECT_EQ(tasks, 1);
}

TEST_F(RunLoopFibersTest, GoGroup) {
  sched::RunLoop loop;

  const size_t kFibers = 7;

  for (size_t i = 0; i < kFibers; ++i) {
    Go(loop, [] {});
  }

  size_t tasks = loop.Run();
  EXPECT_EQ(tasks, kFibers);
}

TEST_F(RunLoopFibersTest, GoChild) {
  sched::RunLoop loop;

  bool flag = false;

  Go(loop, [&] {
    Go([&] {
      flag = true;
    });
  });

  EXPECT_TRUE(loop.RunNext());
  EXPECT_FALSE(flag);
  EXPECT_EQ(loop.Run(), 1);
  EXPECT_TRUE(flag);
}

TEST_F(RunLoopFibersTest, BaseYield) {
  sched::RunLoop loop;

  Go(loop, [] {
    Yield();
  });

  EXPECT_EQ(loop.Run(), 2);
}

TEST_F(RunLoopFibersTest, PingPong) {
  sched::RunLoop loop;

  int turn = 0;

  Go(loop, [&] {
    for (size_t i = 0; i < 3; ++i) {
      EXPECT_EQ(turn, 0);
      turn ^= 1;
      Yield();
    }
  });

  Go(loop, [&] {
    for (size_t j = 0; j < 3; ++j) {
      EXPECT_EQ(turn, 1);
      turn ^= 1;
      Yield();
    }
  });

  loop.Run();
}

TEST_F(RunLoopFibersTest, YieldGroup) {
  sched::RunLoop loop;

  const size_t kFibers = 3;
  const size_t kYields = 4;

  for (size_t i = 0; i < kFibers; ++i) {
    Go(loop, [] {
      for (size_t k = 0; k < kYields; ++k) {
        Yield();
      }
    });
  }

  size_t tasks = loop.Run();
  EXPECT_EQ(tasks, kFibers * (kYields + 1));
}
