#include <Foundation/Systems/Picture.hpp>

using namespace Picture;

Capability Capability::combine(const Capability& a, const Capability& b) {
  return {
      a.enabled && b.enabled,
      a.errorRate + b.errorRate,
  };
}

void Buffer::send(const glm::vec3& v) {
  pictureData = v;
}

std::optional<glm::vec3> Buffer::receive() {
  std::optional<glm::vec3> res = pictureData;
  clear();
  return res;
}

void Buffer::clear() {
  pictureData = std::nullopt;
}
