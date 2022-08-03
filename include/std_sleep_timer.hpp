// File: std_sleep_timer.hpp
// Project: include
// Created Date: 02/08/2022
// Author: Shun Suzuki
// -----
// Last Modified: 02/08/2022
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022 Shun Suzuki. All rights reserved.
//

#pragma once

#include <thread>

#include "timer.hpp"

template <typename T>
class StdSleepTimer : public Timer<T> {
 public:
  StdSleepTimer() : _thread_running(false) {}

  void start(T* callback, uint32_t interval_ns) override {
    _thread_running = true;
    const auto interval = std::chrono::microseconds(interval_ns / 1000);
    _timer_callback = std::thread([this, interval, callback]() {
      auto next = std::chrono::high_resolution_clock::now();
      while (_thread_running) {
        next += interval;
        std::this_thread::sleep_until(next);
        callback->callback();
      }
    });
  }

  void stop() override {
    if (!_thread_running) return;
    _thread_running = false;
    if (_timer_callback.joinable()) _timer_callback.join();
  }

  ~StdSleepTimer() { stop(); }

 private:
  std::thread _timer_callback;
  bool _thread_running;
};
