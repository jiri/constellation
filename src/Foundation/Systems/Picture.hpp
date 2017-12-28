#pragma once

#include <optional>
#include <unordered_map>

#include <glm/glm.hpp>

#include <Foundation/Systems/System.hpp>

class VideoSystem : public System {
  using Buffer = std::optional<glm::vec3>;
public:
  explicit VideoSystem(Universe* u)
    : System { u, "Video", 60 }
  { }

  bool filter(const Connection& edge) const override;
  void swap(Connection& edge) override;

  void send(Endpoint* port, const glm::vec3& v);
  std::optional<glm::vec3> receive(Endpoint* port);

  std::unordered_map<Endpoint*, Buffer> buffers;
};
