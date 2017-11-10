#include <Graphics/Program.hpp>

/* Helper functions */
namespace {
  GLenum guessTypeFromPath(fs::path p) {
    auto ex = p.extension();

    if (ex == ".vert" || ex == ".vsh") { return GL_VERTEX_SHADER;   }
    if (ex == ".frag" || ex == ".fsh") { return GL_FRAGMENT_SHADER; }

    throw std::invalid_argument {
        fmt::format("Unrecognized shader file extension '{}'", ex.string())
    };
  }

  std::string slurp_file(fs::path p) {
    fs::ifstream file { p };

    if (!file.is_open()) {
      throw std::runtime_error {
          fmt::format("Failed to open file '{}'", p.string())
      };
    }

    return {
        std::istreambuf_iterator<char> { file },
        std::istreambuf_iterator<char> {      },
    };
  }
}

/* Shader */
Shader::Shader(const char *p)
  : Shader { fs::path(p) }
{ }

Shader::Shader(const fs::path& path)
  : Shader { path, guessTypeFromPath(path) }
{ }

Shader::Shader(const fs::path& path, GLenum type)
  : id { glCreateShader(type) }
{
  const auto src = slurp_file(path);
  const auto psrc = src.c_str();

  glShaderSource(id, 1, &psrc, nullptr);
  glCompileShader(id);

  GLint success;
  glGetShaderiv(id, GL_COMPILE_STATUS, &success);

  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(id, 512, nullptr, infoLog);

    throw std::runtime_error {
        fmt::format("Compilation of shader '{}' failed:\n{}", path.string(), infoLog)
    };
  }
}

Shader::Shader(Shader &&other) noexcept
  : id { 0 }
{
  std::swap(id, other.id);
}

Shader::~Shader() {
  glDeleteShader(id);
}

Shader &Shader::operator=(Shader other) {
  std::swap(id, other.id);
  return *this;
}

/* Program */
Program::Program(fs::path vpath, fs::path fpath)
  : id { 0 }
  , vertex_path { std::move(vpath) }
  , fragment_path { std::move(fpath) }
{
  reload();
}

Program::Program(Program &&other) noexcept
  : id { 0 }
{
  std::swap(id, other.id);
  std::swap(vertex_path, other.vertex_path);
  std::swap(fragment_path, other.fragment_path);
}

Program& Program::operator=(Program other) {
  std::swap(id, other.id);
  std::swap(vertex_path, other.vertex_path);
  std::swap(fragment_path, other.fragment_path);
  return *this;
}

Program::~Program() {
  glDeleteProgram(id);
}

void Program::reload() {
  if (id != 0) {
    glDeleteProgram(id);
  }

  id = glCreateProgram();

  Shader vsh { vertex_path };
  Shader fsh { fragment_path };

  glAttachShader(id, vsh);
  glAttachShader(id, fsh);

  glLinkProgram(id);

  glDetachShader(id, vsh);
  glDetachShader(id, fsh);

  GLint success;
  glGetProgramiv(id, GL_LINK_STATUS, &success);

  if (!success) {
    GLchar infoLog[512];
    glGetProgramInfoLog(id, 512, nullptr, infoLog);

    throw std::runtime_error {
        fmt::format("Program linking failed:\n{}", infoLog)
    };
  }
}

template<typename T>
void Program::setUniform(GLint location, const T &val) {
  throw std::domain_error {
      fmt::format("Program::setUniform not implemented\n")
  };
}

GLint Program::location(const std::string &name) {
  return glGetUniformLocation(id, name.c_str());
}
