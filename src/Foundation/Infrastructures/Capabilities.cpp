#include <Foundation/Infrastructures/Capabilities.hpp>

#include <algorithm>

Capabilities Capabilities::combine(const Capabilities& a, const Capabilities& b) {
  return {
      {
          a.video.enabled && b.video.enabled,
          a.video.errorRate + b.video.errorRate,
      },
      {
          a.energy.enabled && b.energy.enabled,
          std::min(a.energy.throughput, b.energy.throughput),
      },
      {
          a.text.enabled && b.text.enabled,
      },
  };
}

Capabilities Capabilities::operator*(const Capabilities& other) const {
  return Capabilities::combine(*this, other);
}
