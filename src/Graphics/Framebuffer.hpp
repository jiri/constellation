#pragma once

#include <glad.h>

class Framebuffer {
public:
  Framebuffer(uint32_t width, uint32_t height);
  ~Framebuffer();

  void setViewPort() const;

  GLuint texture() const {
    return this->colorAttachment;
  }

  operator GLuint() {
    return this->fbo;
  }

private:
  GLuint fbo;
  GLuint rbo;
  GLuint colorAttachment;

  uint32_t width;
  uint32_t height;
};
