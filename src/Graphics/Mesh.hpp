#pragma once

#include <vector>
#include <glad.h>

#include <Graphics/Geometry.hpp>

class Mesh {
public:
  explicit Mesh(const Geometry& g);

private:
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
};
