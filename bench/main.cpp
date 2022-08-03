// File: main.cpp
// Project: bench
// Created Date: 02/08/2022
// Author: Shun Suzuki
// -----
// Last Modified: 03/08/2022
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022 Shun Suzuki. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <vector>

#include "std_sleep_timer.hpp"

#ifdef WIN32
#include "win/multimedia_timer.hpp"
#include "win/performance_counter_soft_sleep_timer.hpp"
#include "win/threadpool_timer.hpp"
#include "win/timerqueue_timer.hpp"
#include "win/waitable_timer.hpp"
#elif __APPLE__
#include "mac/gcd_timer.hpp"
#else
#include "nix/posix_timer.hpp"
#endif

constexpr uint32_t NANO_SEC = 1;
constexpr uint32_t MICRO_SEC = NANO_SEC * 1000;
constexpr uint32_t MILLI_SEC = MICRO_SEC * 1000;

constexpr size_t ITERATION = 1000;

#define USE_TIMEBEGINPERIOD

#ifdef WIN32
#include <Windows.h>
#endif

struct Callback {
  std::vector<int64_t> stats;
  std::chrono::time_point<std::chrono::high_resolution_clock> begin;

  void init() {
    stats.clear();
    stats.reserve(ITERATION);
    begin = std::chrono::high_resolution_clock::now();
  }

  void callback() {
    const auto now = std::chrono::high_resolution_clock::now();
    const auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(now - begin);
    stats.emplace_back(dur.count());
  }
};

int main() {
#ifdef WIN32
#ifdef USE_TIMEBEGINPERIOD
  timeBeginPeriod(1);
#endif
#endif

  const std::vector intervals_ns = {MICRO_SEC,           100 * MICRO_SEC,     500 * MICRO_SEC, MILLI_SEC,
                                    16666667 * NANO_SEC, 33333333 * NANO_SEC, 100 * MILLI_SEC};

  std::vector<std::pair<std::unique_ptr<Timer<Callback>>, std::string>> timers;
  timers.emplace_back(std::make_unique<StdSleepTimer<Callback>>(), "StdSleepTimer");
#ifdef WIN32
  timers.emplace_back(std::make_unique<MultiMediaTimer<Callback>>(), "MultiMediaTimer");
  timers.emplace_back(std::make_unique<TimerQueueTimer<Callback>>(), "TimerQueueTimer");
  timers.emplace_back(std::make_unique<ThreadPoolTimer<Callback>>(), "ThreadPoolTimer");
  timers.emplace_back(std::make_unique<WaitableTimer<Callback>>(), "WaitableTimer");
  timers.emplace_back(std::make_unique<PerformanceCounterSoftSleepTimer<Callback>>(), "PerformanceCounterSoftSleepTimer");
#elif __APPLE__
  timers.emplace_back(std::make_unique<GCDTimer<Callback>>(), "GCDTimer");
#else
  timers.emplace_back(std::make_unique<POSIXTimer<Callback>>(), "POSIXTimer");
#endif

  std::ofstream of;
  of.open("stats.csv", std::ios::out);

  Callback callback{};

  for (auto& [timer, name] : timers) {
    std::cout << "Testing: " << name << std::endl;
    for (const auto interval : intervals_ns) {
      std::cout << "\tinterval: " << interval << " [ns]" << std::endl;

      callback.init();
      timer->start(&callback, interval);
      while (callback.stats.size() < ITERATION) std::this_thread::sleep_for(std::chrono::nanoseconds(interval * ITERATION));
      timer->stop();

      of << name << ", " << interval;
      for (size_t i = 0; i < ITERATION; i++) of << ", " << callback.stats[i];
      of << std::endl;
    }
  }

  of.close();

#ifdef WIN32
#ifdef USE_TIMEBEGINPERIOD
  timeEndPeriod(1);
#endif
#endif

  return 0;
}
