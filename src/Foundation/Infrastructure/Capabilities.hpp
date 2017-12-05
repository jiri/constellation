#pragma once

#include <limits>

struct Capabilities {
  struct {
    bool enabled = true;
    float errorRate = 0.0f;
  } picture;

  struct {
    bool enabled = true;
    float throughput = std::numeric_limits<float>::infinity();
  } energy;

  struct {
    bool enabled = true;
  } text;

  static Capabilities combine(const Capabilities& a, const Capabilities& b);
};
