#include <Graphics/Mesh.hpp>

#include <glm/gtc/type_ptr.hpp>

Mesh::Mesh(const Geometry& g)
  : vao { 0 }
  , vbo { 0 }
  , ebo { 0 }
  , numIndices { g.indices.size() }
{
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, g.vertices.size() * sizeof(Vertex), g.vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, g.indices.size() * sizeof(uint32_t), g.indices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, materialId));

  glBindVertexArray(0);
}

void Mesh::draw() const {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, this->numIndices, GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}
