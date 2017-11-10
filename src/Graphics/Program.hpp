#pragma once

#include <string>
#include <boost/filesystem.hpp>

#include <glad.h>

#include <fmt/format.h>

namespace fs = boost::filesystem;

class Shader {
public:
  Shader(const char *path);
  Shader(const fs::path &path);
  Shader(const fs::path &path, GLenum type);

  Shader(const Shader &) = delete;
  Shader(Shader &&other) noexcept;
  ~Shader();

  Shader& operator=(Shader other);

  operator GLuint() const {
    return id;
  }

private:
  GLuint id;
};

class Program {
public:
  Program(fs::path vpath, fs::path fpath);

  Program(const Program &) = delete;
  Program(Program &&other) noexcept;

  ~Program();

  Program& operator=(Program other);

  void reload();

  template<typename T>
  void setUniform(const std::string& name, const T& val) {
    setUniform(location(name), val);
  }

  operator GLuint() const {
    return id;
  }

private:
  template <typename T>
  static void setUniform(GLint location, const T& val);

  GLint location(const std::string& name);

  GLuint id;

  fs::path vertex_path;
  fs::path fragment_path;
};
