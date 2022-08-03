// File: waitable_timer.hpp
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
class WaitableTimer : public Timer<T> {
 public:
  WaitableTimer() : _thread_running(false), _timer(nullptr) {}

  void start(T* callback, uint32_t interval_ns) override {
    _timer = CreateWaitableTimer(nullptr, TRUE, nullptr);

    const LONGLONG interval = static_cast<LONGLONG>(interval_ns) / 100;
    _thread_running = true;
    _timer_callback = std::thread([this, interval, callback]() {
      FILETIME system_time;
      GetSystemTimePreciseAsFileTime(&system_time);
      LARGE_INTEGER next_time = *reinterpret_cast<LARGE_INTEGER*>(&system_time);
      while (_thread_running) {
        next_time.QuadPart += interval;
        SetWaitableTimer(_timer, &next_time, 0, nullptr, nullptr, FALSE);
        if (WaitForSingleObject(_timer, INFINITE) != WAIT_OBJECT_0) break;
        callback->callback();
      }
    });
  }
  void stop() override {
    if (!_thread_running) return;
    _thread_running = false;
    if (_timer_callback.joinable()) _timer_callback.join();
    CloseHandle(_timer);
    _timer = nullptr;
  }

  ~WaitableTimer() { stop(); }

 private:
  std::thread _timer_callback;
  bool _thread_running;
  HANDLE _timer;
};
