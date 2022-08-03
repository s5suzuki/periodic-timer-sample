// File: gcd_timer.hpp
// Project: mac
// Created Date: 03/08/2022
// Author: Shun Suzuki
// -----
// Last Modified: 03/08/2022
// Modified By: Shun Suzuki (suzuki@hapis.k.u-tokyo.ac.jp)
// -----
// Copyright (c) 2022 Shun Suzuki. All rights reserved.
//

#pragma once

#include <dispatch/dispatch.h>

#include <thread>

#include "timer.hpp"

template <typename T>
class GCDTimer : public Timer<T> {
 public:
  GCDTimer() : _queue(nullptr), _timer(nullptr), _stopped(false) {}

  void start(T* callback, uint32_t interval_ns) override {
    _queue = dispatch_queue_create("timerQueue", 0);

    _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, _queue);
    dispatch_source_set_event_handler(_timer, ^{
      _callback(callback);
    });

    dispatch_source_set_cancel_handler(_timer, ^{
      dispatch_release(_timer);
      dispatch_release(_queue);
    });

    dispatch_time_t start = dispatch_time(DISPATCH_TIME_NOW, 0);
    dispatch_source_set_timer(_timer, start, interval_ns, 0);
    dispatch_resume(_timer);
    _stopped = false;
  }

  void stop() override {
    if (_stopped) return;
    dispatch_source_cancel(_timer);
    _stopped = true;
  }

  ~GCDTimer() { stop(); }

 private:
  void _callback(T* ptr) { ptr->callback(); }

  dispatch_queue_t _queue;
  dispatch_source_t _timer;
  bool _stopped;
};
