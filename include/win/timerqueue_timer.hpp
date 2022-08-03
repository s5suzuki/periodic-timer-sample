// File: timerqueue_timer.hpp
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
class TimerQueueTimer : public Timer<T> {
 public:
  TimerQueueTimer() : _timer_queue(nullptr), _timer(nullptr) {}

  void start(T* callback, uint32_t interval_ns) override {
    const auto interval = interval_ns / 1000 / 1000;
    _timer_queue = CreateTimerQueue();
    CreateTimerQueueTimer(&_timer, _timer_queue, (WAITORTIMERCALLBACK)timer_callback, (void*)callback, 0, std::max(1u, interval), 0);
  }

  void stop() override {
    if (_timer == nullptr) return;
    if (_timer_queue == nullptr) return;
    DeleteTimerQueueTimer(_timer_queue, _timer, nullptr);
    DeleteTimerQueue(_timer_queue);
    _timer = nullptr;
    _timer_queue = nullptr;
  }

  ~TimerQueueTimer() { stop(); }

 private:
  static void CALLBACK timer_callback(PVOID lpParam, BOOLEAN TimerOrWaitFired) { reinterpret_cast<T*>(lpParam)->callback(); }

  HANDLE _timer_queue;
  HANDLE _timer;
};
