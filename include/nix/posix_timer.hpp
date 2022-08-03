// File: posix_timer.hpp
// Project: nix
// Created Date: 03/08/2022
// Author: Shun Suzuki
// -----
// Last Modified: 03/08/2022
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022 Shun Suzuki. All rights reserved.
//

#pragma once

#include <signal.h>
#include <time.h>

#include <cstring>
#include <thread>

#include "timer.hpp"

template <typename T>
class POSIXTimer : public Timer<T> {
 public:
  POSIXTimer() : _timer(nullptr) {}

  void start(T* callback, uint32_t interval_ns) override {
    struct itimerspec itval;
    struct sigevent se;

    itval.it_value.tv_sec = 0;
    itval.it_value.tv_nsec = interval_ns;
    itval.it_interval.tv_sec = 0;
    itval.it_interval.tv_nsec = interval_ns;

    memset(&se, 0, sizeof(se));
    se.sigev_value.sival_ptr = callback;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_function = _callback;
    se.sigev_notify_attributes = NULL;

    timer_create(CLOCK_REALTIME, &se, &_timer);
    timer_settime(_timer, 0, &itval, NULL);
  }

  void stop() override {
    if (_timer == nullptr) return;
    timer_delete(_timer);
    _timer = nullptr;
  }

  ~POSIXTimer() { stop(); }

 private:
  static void _callback(union sigval sv) { reinterpret_cast<T*>(sv.sival_ptr)->callback(); }

  timer_t _timer;
};
