#pragma once

#include <vector>
#include <glad.h>

#include <Graphics/Geometry.hpp>
#include <Graphics/Camera.hpp>
#include <Graphics/Program.hpp>

class Mesh {
public:
  Mesh(const Geometry& g);

  Mesh(const Mesh&) = delete;
  Mesh(Mesh&&) = default;

  void draw() const;

private:
  GLuint vao;
  GLuint vbo;
  GLuint ebo;

  size_t numIndices;
};
