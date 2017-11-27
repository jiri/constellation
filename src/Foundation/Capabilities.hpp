#pragma once

struct Capabilities {
  struct {
    bool enabled = false;
    float errorRate = 0.0f;
  } picture;

  struct {
    bool enabled = false;
    float throughput = 0.0f;
  } energy;

  struct {
    bool enabled = false;
  } text;

  static Capabilities combine(const Capabilities& a, const Capabilities& b);
};
