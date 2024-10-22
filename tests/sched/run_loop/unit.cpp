#include <fiber/sched/run_loop.hpp>
#include <fiber/sched/task/submit.hpp>

#include <gtest/gtest.h>
#include <fmt/core.h>

using namespace fiber;  // NOLINT

class RunLoopTest : public ::testing::Test {};

TEST_F(RunLoopTest, JustWorks) {
  sched::RunLoop loop;

  size_t step = 0;

  EXPECT_FALSE(loop.NonEmpty());

  EXPECT_FALSE(loop.RunNext());
  EXPECT_EQ(loop.RunAtMost(99), 0);

  sched::task::Submit(loop, [&] {
    step = 1;
  });

  EXPECT_TRUE(loop.NonEmpty());
  EXPECT_EQ(step, 0);

  sched::task::Submit(loop, [&] {
    step = 2;
  });

  EXPECT_EQ(step, 0);
  EXPECT_TRUE(loop.RunNext());
  EXPECT_EQ(step, 1);
  EXPECT_TRUE(loop.NonEmpty());

  sched::task::Submit(loop, [&] {
    step = 3;
  });

  EXPECT_EQ(loop.RunAtMost(99), 2);
  EXPECT_EQ(step, 3);
  EXPECT_FALSE(loop.NonEmpty());
  EXPECT_FALSE(loop.RunNext());
}

TEST_F(RunLoopTest, Empty) {
  sched::RunLoop loop;

  EXPECT_FALSE(loop.RunNext());
  EXPECT_EQ(loop.RunAtMost(7), 0);
  EXPECT_EQ(loop.Run(), 0);
}

void Countdown(sched::RunLoop& loop, size_t k) {
  if (k > 0) {
    sched::task::Submit(loop, [&loop, k] {
      Countdown(loop, k - 1);
    });
  }
}

TEST_F(RunLoopTest, RunAtMost) {
  sched::RunLoop loop;

  Countdown(loop, 256);

  size_t tasks = 0;
  do {
    tasks += loop.RunAtMost(7);
  } while (loop.NonEmpty());

  fmt::println("{}", tasks);

  EXPECT_EQ(tasks, 256);
}

TEST_F(RunLoopTest, RunAtMostNewTasks) {
  sched::RunLoop loop;

  sched::task::Submit(loop, [&]() {
    sched::task::Submit(loop, []() {});
  });

  EXPECT_EQ(loop.RunAtMost(2), 2);
}

TEST_F(RunLoopTest, Run) {
  sched::RunLoop loop;

  Countdown(loop, 117);

  EXPECT_EQ(loop.Run(), 117);
}

TEST_F(RunLoopTest, RunTwice) {
  sched::RunLoop loop;

  Countdown(loop, 11);

  EXPECT_EQ(loop.Run(), 11);

  Countdown(loop, 7);

  EXPECT_EQ(loop.Run(), 7);
}
