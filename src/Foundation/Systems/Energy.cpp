#include <Foundation/Systems/Energy.hpp>

#include <algorithm>

using namespace Energy;

Capability Capability::combine(const Capability& a, const Capability& b) {
  return {
      a.enabled && b.enabled,
      std::min(a.throughput, b.throughput),
  };
}

void Buffer::offer(float energy) {
  energyOffer += energy;
}

float Buffer::request(float req) {
  float res = std::min(energyPool, req);
  energyPool -= res;
  return res;
}

void Buffer::clear() {
  energyOffer = 0.0f;
}
