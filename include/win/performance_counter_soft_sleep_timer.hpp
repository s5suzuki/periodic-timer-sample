// File: performance_counter_soft_sleep_timer.hpp
// Project: win
// Created Date: 02/08/2022
// Author: Shun Suzuki
// -----
// Last Modified: 03/08/2022
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022 Shun Suzuki. All rights reserved.
//

#pragma once

#include <Windows.h>

#include <thread>

#include "timer.hpp"

template <typename T>
class PerformanceCounterSoftSleepTimer : public Timer<T> {
 public:
  PerformanceCounterSoftSleepTimer() : _thread_running(false) {}

  void start(T* callback, uint32_t interval_ns) override {
    LARGE_INTEGER f;
    QueryPerformanceFrequency(&f);
    LONGLONG interval = (f.QuadPart / 1000LL / 1000LL * interval_ns) / 1000LL;
    _thread_running = true;
    _timer_callback = std::thread([this, interval, callback]() {
      LARGE_INTEGER next_time, now;
      QueryPerformanceCounter(&next_time);
      while (_thread_running) {
        next_time.QuadPart += interval;
        while (true) {
          QueryPerformanceCounter(&now);
          if (now.QuadPart >= next_time.QuadPart) break;
        }
        callback->callback();
      }
    });
  }

  void stop() override {
    if (!_thread_running) return;
    _thread_running = false;
    if (_timer_callback.joinable()) _timer_callback.join();
  }

  ~PerformanceCounterSoftSleepTimer() { stop(); }

 private:
  std::thread _timer_callback;
  bool _thread_running;
};
