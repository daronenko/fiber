#include <fiber/sync/wait_group.hpp>
#include <fiber/sched/thread_pool.hpp>
#include <fiber/core/sched/go.hpp>

#include <wheels/core/stop_watch.hpp>

#include <iostream>
#include <thread>
#include <chrono>
#include <cassert>

using namespace std::chrono_literals;
using namespace fiber;

int main() {
    const size_t kThreads = 4;

    sched::ThreadPool pool{kThreads};
    pool.Start();

    wheels::StopWatch stop_watch;

    sync::WaitGroup wg;

    for (size_t i = 0; i < kThreads; ++i) {
      wg.Add(1);
      core::Go(pool, [&wg, i] {
        std::this_thread::sleep_for(1s);
        wg.Done();
      });
    }

    wg.Wait();

    assert(stop_watch.Elapsed() < 2s);

    pool.Stop();

    return 0;
}