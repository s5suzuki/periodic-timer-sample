// File: multimedia_timer.hpp
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
class MultiMediaTimer : public Timer<T> {
 public:
  MultiMediaTimer() : _timer_id(0) {}

  void start(T* callback, uint32_t interval_ns) override {
    const auto interval = interval_ns / 1000 / 1000;
    _timer_id = timeSetEvent(std::max(1u, interval), 1, timer_callback, reinterpret_cast<DWORD_PTR>(callback),
                             TIME_PERIODIC | TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS);
  }

  void stop() override {
    if (_timer_id == 0) return;
    timeKillEvent(_timer_id);
    _timer_id = 0;
  }

  ~MultiMediaTimer() { stop(); }

 private:
  static void CALLBACK timer_callback(UINT, UINT, DWORD_PTR dw_user, DWORD_PTR, DWORD_PTR) { reinterpret_cast<T*>(dw_user)->callback(); }

  uint32_t _timer_id;
};
