#include <Foundation/Infrastructures/Capabilities.hpp>

#include <algorithm>

Capabilities Capabilities::combine(const Capabilities& a, const Capabilities& b) {
  return {
      {
          a.picture.enabled && b.picture.enabled,
          a.picture.errorRate + b.picture.errorRate,
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
