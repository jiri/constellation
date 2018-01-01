#include <Graphics/Geometry.hpp>

#include <tiny_obj_loader.h>
#include <glad.h>

#include <fmt/format.h>

const Geometry Geometry::CUBE {
    {
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f,  0.0f, -1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },

        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f,  0.0f,  1.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },

        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 { -1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },

        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  1.0f,  0.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },

        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f, -0.5f,  0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f, -0.5f, -0.5f }, glm::vec3 {  0.0f, -1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },

        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 {  0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f,  0.5f,  0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
        Vertex { glm::vec3 { -0.5f,  0.5f, -0.5f }, glm::vec3 {  0.0f,  1.0f,  0.0f }, glm::vec2 { 0.0f, 0.0f }, 0 },
    },
};

Geometry Geometry::load(const fs::path& filename) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), (filename.parent_path() / "/").c_str());

  if (!ret || !err.empty()) {
    throw std::runtime_error { err };
  }

  std::vector<Vertex> vertices;

  for (const auto& shape : shapes) {
    size_t index_offset = 0;

    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
      int fv = shape.mesh.num_face_vertices[f];

      /* Loop over vertices in the face */
      for (size_t v = 0; v < fv; v++) {
        tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

        Vertex vert {
            glm::vec3 {
                attrib.vertices[3 * idx.vertex_index + 0],
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2],
            },
            glm::vec3 {
                attrib.normals[3 * idx.normal_index + 0],
                attrib.normals[3 * idx.normal_index + 1],
                attrib.normals[3 * idx.normal_index + 2],
            },
            glm::vec2 {
                attrib.texcoords.empty() ? 0.0f : attrib.texcoords[2 * idx.texcoord_index + 0],
                attrib.texcoords.empty() ? 0.0f : attrib.texcoords[2 * idx.texcoord_index + 1],
            },
            static_cast<float>(shape.mesh.material_ids[f]),
        };

        vertices.push_back(vert);
      }

      index_offset += fv;
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
