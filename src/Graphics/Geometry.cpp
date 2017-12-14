#include <Graphics/Geometry.hpp>

const Geometry Geometry::CUBE {
    {
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f } },
        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f } },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f } },

        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f } },
        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f } },
        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f } },

        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f } },

        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f } },

        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f } },

        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f } },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f } },
        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f } },
    },
};

Geometry Geometry::scale(float scale) const {
  std::vector<Vertex> vs;
  for (Vertex& v : vs) {
    v.position *= scale;
  }
  return { vs, indices };
}
