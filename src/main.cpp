#include <boost/graph/adjacency_list.hpp>
#include <fmt/format.h>
#include <gsl/gsl>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <Graphics/Program.hpp>

struct Component;
struct Port;

struct Vertex {
  Component* component = nullptr;
};

struct Properties {
  bool picture = true;
  float power = std::numeric_limits<float>::max();
};

struct EdgeProperties : public Properties {
  Port* a;
  Port* b;
};

Properties combine(const Properties& a, const Properties& b) {
  return { a.picture && b.picture, std::min(a.power, b.power) };
}

struct vertex_property_t {
  typedef boost::edge_property_tag kind;
};
struct edge_property_t {
  typedef boost::edge_property_tag kind;
};

typedef boost::adjacency_list<boost::vecS,
    boost::vecS,
    boost::undirectedS,
    boost::property<vertex_property_t, Vertex>,
    boost::property<edge_property_t, EdgeProperties>> CommunicationGraph;
static CommunicationGraph G;

struct Node {
  explicit Node(Properties properties)
      : properties(properties) {}

  virtual ~Node() = default;

  Properties properties;
};

struct Port : public Node {
  using Node::Node;
  CommunicationGraph::vertex_descriptor vertex;
  Node* neighbour = nullptr;
  std::string data;
};

struct Cable : public Node {
  using Node::Node;
  Node* neighbours[2] {};
  uint8_t neighbourCount = 0;
};

struct Component {
  Component()
      : vertex(boost::add_vertex(G)) {}

  virtual ~Component() {
    boost::remove_vertex(this->vertex, G);
  }

  virtual void update() = 0;

  virtual void render() const = 0;

  CommunicationGraph::vertex_descriptor vertex;
};

struct Monitor : public Component {
  std::string picture;

  Monitor()
      : Component() {
    this->port.vertex = this->vertex;
  }

  void update() override {
    this->picture = this->port.data;
    if (this->picture.empty()) {
      this->picture = "askjnsdgdsae";
    }
  }

  void render() const override {
    fmt::print("{}\n", this->picture);
  }

  Port port { Properties { true, 1.0f }};
};

struct Camera : public Component {
  Camera()
      : Component() {
    this->port.vertex = this->vertex;
  }

  void update() override {
    static uint32_t counter = 0;
    this->port.data = fmt::format("HELLO {}", counter++);
  }

  void render() const override {}

  Port port { Properties { true, 1.0f }};
};

bool connect(Node* a, Node* b) {
  if (auto* pa = dynamic_cast<Port*>(a)) {
    if (pa->neighbour) {
      return false;
    }
    if (auto* pb = dynamic_cast<Port*>(b)) {
      return false;
    }
    else if (auto* cb = dynamic_cast<Cable*>(b)) {
      if (cb->neighbourCount >= 2) {
        return false;
      }
      pa->neighbour = cb;
      cb->neighbours[cb->neighbourCount++] = pa;
      goto end;
    }
    throw;
  }
  else if (auto* ca = dynamic_cast<Cable*>(a)) {
    if (ca->neighbourCount >= 2) {
      return false;
    }
    if (auto* pb = dynamic_cast<Port*>(b)) {
      if (pb->neighbour) {
        return false;
      }
      pb->neighbour = ca;
      ca->neighbours[ca->neighbourCount++] = pb;
      goto end;
    }
    else if (auto* cb = dynamic_cast<Cable*>(b)) {
      if (cb->neighbourCount >= 2) {
        return false;
      }
      ca->neighbours[ca->neighbourCount++] = cb;
      cb->neighbours[cb->neighbourCount++] = ca;
      goto end;
    }
    throw;
  }
  throw;
  end:
  Port* left = nullptr;
  Port* right = nullptr;
  Properties result {};

  Node* index = a;
  Node* prev = b;

  while (index) {
    result = combine(result, index->properties);
    if (auto* pi = dynamic_cast<Port*>(index)) {
      left = pi;
      break;
    }
    else if (auto* ci = dynamic_cast<Cable*>(index)) {
      index = ci->neighbours[0] == prev ? ci->neighbours[1] : ci->neighbours[0];
      prev = ci;
      continue;
    }
    break;
  }

  index = b;
  prev = a;

  while (index) {
    result = combine(result, index->properties);
    if (auto* pi = dynamic_cast<Port*>(index)) {
      right = pi;
      break;
    }
    else if (auto* ci = dynamic_cast<Cable*>(index)) {
      index = ci->neighbours[0] == prev ? ci->neighbours[1] : ci->neighbours[0];
      prev = ci;
      continue;
    }
    throw;
  }

  if (left && right) {
    boost::add_edge(left->vertex, right->vertex, boost::property<edge_property_t, EdgeProperties>{
        EdgeProperties { result.picture, result.power, left, right }}, G);
  }

  return true;
}

bool connect(Node& a, Node &b) {
  return connect(&a, &b);
}

void update() {
  boost::property_map<CommunicationGraph, edge_property_t>::type EdgePropertyMap = boost::get(edge_property_t(), G);

  for (auto edgePair = boost::edges(G); edgePair.first != edgePair.second; ++edgePair.first) {
    auto& prop = EdgePropertyMap[*edgePair.first];
    if (prop.picture) {
      std::swap(prop.a->data, prop.b->data);
    }
  }
}

int main() {
  glfwSetErrorCallback([](int, const char* message) {
    fmt::print("{}\n", message);
  });

  glfwInit();
  auto terminateGlfw = gsl::finally([] { glfwTerminate(); });

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(800, 600, "", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  if(!gladLoadGL()) {
    printf("Something went wrong!\n");
    exit(-1);
  }

  glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
  });

  /* Enable VSync */
  glfwSwapInterval(0);

  /* Initialise ImGui */
  ImGui_ImplGlfwGL3_Init(window, true);
  auto shutdownImgui = gsl::finally([] { ImGui_ImplGlfwGL3_Shutdown(); });

  /* Data */
  float vertices[] = {
          -0.5f, -0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
          0.0f,  0.5f, 0.0f
  };

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  Program prog { "shd/basic.vert", "shd/basic.frag" };

  /* Main loop */
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    /* Rendering */
    glClearColor(0.17f, 0.24f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(prog);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);

    ImGui::Begin("Hello");
    ImGui::Text("world");
    ImGui::End();

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  return 0;
}
