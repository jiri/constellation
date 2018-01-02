#include <Graphics/Framebuffer.hpp>

#include <stdexcept>

Framebuffer::Framebuffer(uint32_t width, uint32_t height)
  : width { width }
  , height { height }
{
  glGenFramebuffers(1, &this->fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

  /* Color buffer */
  glGenTextures(1, &this->colorAttachment);
  glBindTexture(GL_TEXTURE_2D, this->colorAttachment);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorAttachment, 0);

  /* Depth buffer */
  glGenRenderbuffers(1, &this->rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

  /* Error checking */
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error { "Framebuffer is not complete" };
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
  glDeleteFramebuffers(1, &this->fbo);
}

void Framebuffer::setViewPort() const {
  glViewport(0, 0, this->width, this->height);
}
