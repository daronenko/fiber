#include <fiber/sched/run_loop.hpp>

#include <gtest/gtest.h>
#include <fmt/core.h>

using namespace fiber;  // NOLINT

class RunLoopTest : public ::testing::Test {};

TEST_F(RunLoopTest, JustWorksIntrusive) {
  sched::RunLoop loop;

  bool flag = false;

  class SetFlag : public sched::task::TaskBase {
   public:
    explicit SetFlag(bool& flag)
        : flag_(flag) {
    }

    void Run() noexcept override {
      flag_ = true;
    }

   private:
    bool& flag_;
  };

  {
    SetFlag set{flag};
    loop.Submit(&set);
    loop.RunNext();
  }

  EXPECT_TRUE(flag);
}
