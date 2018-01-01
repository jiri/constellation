#include <Graphics/Geometry.hpp>

#include <tiny_obj_loader.h>
#include <glad.h>

#include <fmt/format.h>

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

Geometry Geometry::load(const fs::path& filename) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());

  if (!ret || !err.empty()) {
    throw std::runtime_error { err };
  }

  std::vector<Vertex> vertices;

  for (const auto &shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vert {
          glm::vec3 {
              attrib.vertices[3 * index.vertex_index + 0],
              attrib.vertices[3 * index.vertex_index + 1],
              attrib.vertices[3 * index.vertex_index + 2],
          },
          glm::vec3 {
              attrib.normals[3 * index.normal_index + 0],
              attrib.normals[3 * index.normal_index + 1],
              attrib.normals[3 * index.normal_index + 2],
          },
//          glm::vec2 {
//              attrib.texcoords[2 * index.texcoord_index + 0],
//              attrib.texcoords[2 * index.texcoord_index + 1],
//          },
      };

      vertices.push_back(vert);
    }
  }

  return Geometry { vertices };
}

Geometry Geometry::scale(float scale) const {
  std::vector<Vertex> vs;
  for (Vertex& v : vs) {
    v.position *= scale;
  }
  return { vs, indices };
}
