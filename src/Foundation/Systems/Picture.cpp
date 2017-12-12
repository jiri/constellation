#include <Foundation/Systems/Picture.hpp>

#include <Util/Random.hpp>

bool PictureSystem::filter(const Connection& edge) const {
  return edge.capabilities.picture.enabled;
}

void PictureSystem::swap(Connection& edge) {
  if (randomFloat() < edge.capabilities.picture.errorRate) {
    buffers[edge.to] = randomColor();
  }
  else {
    buffers[edge.to] = buffers[edge.from];
  }
}

void PictureSystem::send(Port* port, const glm::vec3& v) {
  buffers[port] = v;
}

std::optional<glm::vec3> PictureSystem::receive(Port* port) {
  std::optional<glm::vec3> res = buffers[port];
  buffers[port] = std::nullopt;
  return res;
}
