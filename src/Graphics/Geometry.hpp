#pragma once

#include <vector>
#include <numeric>

#include <glm/glm.hpp>

#include <Util/Filesystem.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  int32_t materialId;
};

class Geometry {
public:
  explicit Geometry(std::vector<Vertex> vs)
    : vertices { std::move(vs) }
    , indices(vertices.size())
  {
    std::iota(indices.begin(), indices.end(), 0);
  }

  Geometry(std::vector<Vertex> vs, std::vector<uint32_t> is)
    : vertices { std::move(vs) }
    , indices { std::move(is) }
  { }

  static Geometry load(const fs::path& filename);

  Geometry scale(float scale) const;

  static const Geometry CUBE;

  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
};
