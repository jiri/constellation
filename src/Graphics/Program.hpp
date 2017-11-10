#pragma once

#include <string>
#include <boost/filesystem.hpp>

#include <glad.h>

#include <fmt/format.h>

class Shader {
public:
  Shader(const char *path);
  Shader(const boost::filesystem::path &path);
  Shader(const boost::filesystem::path &path, GLenum type);

  Shader(const Shader &) = delete;
  Shader(Shader &&other);

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
  Program(boost::filesystem::path vpath, boost::filesystem::path fpath);

  Program(const Program &) = delete;
  Program(Program &&other);

  ~Program();

  Program& operator=(Program other);

  void reload();

  template<typename T>
  void setUniform(std::string name, const T &val) {
    setUniform(location(name), val);
  }

  operator GLuint() const {
    return id;
  }

private:
  template <typename T>
  static void setUniform(GLint location, const T &val);

  GLint location(const std::string &name);

  GLuint id;

  boost::filesystem::path vertex_path;
  boost::filesystem::path fragment_path;
};
