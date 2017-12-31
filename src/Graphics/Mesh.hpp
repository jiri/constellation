#pragma once

#include <vector>
#include <glad.h>

#include <Graphics/Geometry.hpp>
#include <Graphics/Camera.hpp>
#include <Graphics/Program.hpp>

class Mesh {
public:
  Mesh(const Geometry& g);

  void draw(const gl::Camera& c, const Program& p);

private:
  GLuint vao;
  GLuint vbo;
  GLuint ebo;

  size_t numIndices;
};
