// File: threadpool_timer.hpp
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
class ThreadPoolTimer : public Timer<T> {
 public:
  ThreadPoolTimer() : _timer(nullptr) {}

  void start(T* callback, uint32_t interval_ns) override {
    _timer = CreateThreadpoolTimer(timer_callback, (PVOID)callback, NULL);
    ULARGE_INTEGER start{};
    start.QuadPart = (ULONGLONG)(-interval_ns / 100);
    FILETIME ft{};
    ft.dwHighDateTime = start.HighPart;
    ft.dwLowDateTime = start.LowPart;
    const auto interval = interval_ns / 1000 / 1000;
    SetThreadpoolTimer(_timer, &ft, std::max(1u, interval), 0);
  }

  void stop() override {
    if (_timer == nullptr) return;
    CloseThreadpoolTimer(_timer);
    _timer = nullptr;
  }

  ~ThreadPoolTimer() { stop(); }

 private:
  static void NTAPI timer_callback(PTP_CALLBACK_INSTANCE, PVOID Context, PTP_TIMER) { reinterpret_cast<T*>(Context)->callback(); }

  PTP_TIMER _timer;
};
