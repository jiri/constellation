#include <Graphics/Program.hpp>

#include <stack>
#include <fstream>
using namespace std;

using namespace boost::filesystem;

/* Helper functions */
namespace {
  GLenum guessTypeFromPath(path p) {
    auto ex = p.extension();

    if (ex == ".vert" || ex == ".vsh") { return GL_VERTEX_SHADER;   }
    if (ex == ".frag" || ex == ".fsh") { return GL_FRAGMENT_SHADER; }

    throw invalid_argument {
            fmt::format("Unrecognized shader file extension '{}'", ex.string())
    };
  }

  string slurp_file(path p) {
    boost::filesystem::ifstream file { p };

    if (!file.is_open()) {
      throw runtime_error {
              fmt::format("Failed to open file '{}'", p.string())
      };
    }

    return {
            istreambuf_iterator<char> { file },
            istreambuf_iterator<char> {      },
    };
  }
}

/* Shader */
Shader::Shader(const char *p)
        : Shader { path(p) }
{ }

Shader::Shader(const path &path)
        : Shader { path, guessTypeFromPath(path) }
{ }

Shader::Shader(const path &path, GLenum type)
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

    throw runtime_error {
            fmt::format("Compilation of shader '{}' failed:\n{}", path.string(), infoLog)
    };
  }
}

Shader::Shader(Shader &&other)
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
Program::Program(path vpath, path fpath)
        : id { 0 }
        , vertex_path { vpath }
        , fragment_path { fpath }
{
  reload();
}

Program::Program(Program &&other)
        : id { 0 }
{
  std::swap(id, other.id);
  std::swap(vertex_path, other.vertex_path);
  std::swap(fragment_path, other.fragment_path);
}

Program &Program::operator=(Program other) {
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

    throw runtime_error {
            fmt::format("Program linking failed:\n{}", infoLog)
    };
  }
}

template<typename T>
void Program::setUniform(GLint location, const T &val) {
  throw domain_error {
          fmt::format("Program::setUniform not implemented\n")
  };
}

GLint Program::location(const std::string &name) {
  return glGetUniformLocation(id, name.c_str());
}
