#include <Foundation/Systems.hpp>

Capabilities Capabilities::combine(const Capabilities& a, const Capabilities& b) {
  return {
      Picture::Capability::combine(a.picture, b.picture),
      Energy::Capability::combine(a.energy, b.energy),
      Text::Capability::combine(a.text, b.text),
  };
}
