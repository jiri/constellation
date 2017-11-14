#pragma once

namespace Energy {
  struct Capability {
    bool enabled = false;
    float throughput = 0.0f;

    static Capability combine(const Capability& a, const Capability& b);
  };

  struct Buffer {
    float energyOffer = 0.0f;
    float energyPool = 0.0f;

    void offer(float energy);
    float request(float req);

    void clear();
  };
}
