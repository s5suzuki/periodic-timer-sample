// File: timer.hpp
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

#include <cstdint>
#include <functional>

template <typename T>
class Timer {
 public:
  virtual void start(T* callback, uint32_t interval_ns) = 0;
  virtual void stop() = 0;
  virtual ~Timer() {}
};
