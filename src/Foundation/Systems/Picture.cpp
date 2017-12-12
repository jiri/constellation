#include <Foundation/Systems/Picture.hpp>

#include <Util/Random.hpp>

bool PictureSystem::filter(const Connection& edge) const {
  return edge.capabilities.picture.enabled;
}

void PictureSystem::swap(Connection& edge) {
  if (randomFloat() < edge.capabilities.picture.errorRate) {
    buffers[edge.from].pictureData = randomColor();
    buffers[edge.to].pictureData = randomColor();
  }
  std::swap(buffers[edge.from], buffers[edge.to]);
}

void PictureSystem::send(Port* port, const glm::vec3& v) {
  buffers[port].pictureData = v;
}

std::optional<glm::vec3> PictureSystem::receive(Port* port) {
  std::optional<glm::vec3> res = buffers[port].pictureData;
  buffers[port].pictureData = std::nullopt;
  return res;
}
