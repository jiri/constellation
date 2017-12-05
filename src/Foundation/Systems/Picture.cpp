#include <Foundation/Systems/Picture.hpp>

#include <Util/Random.hpp>

bool Picture::System::filter(const Connection& edge) const {
  return edge.capabilities.picture.enabled;
}

void Picture::System::swap(Connection& edge) {
  if (randomFloat() < edge.capabilities.picture.errorRate) {
    buffers[edge.a].pictureData = randomColor();
    buffers[edge.b].pictureData = randomColor();
  }
  std::swap(buffers[edge.a], buffers[edge.b]);
}

void Picture::System::send(Port* port, const glm::vec3& v) {
  buffers[port].pictureData = v;
}

std::optional<glm::vec3> Picture::System::receive(Port* port) {
  std::optional<glm::vec3> res = buffers[port].pictureData;
  buffers[port].pictureData = std::nullopt;
  return res;
}
