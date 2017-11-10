#include <optional>
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

class Port;

struct Node {
  explicit Node(Properties properties)
    : properties(properties) {}

  virtual ~Node() = default;

  virtual Port* findPort(Node* prev = nullptr) = 0;

  virtual bool free() const = 0;

  virtual void addNeighbour(Node* node) = 0;

  virtual std::optional<Properties> fold(Node* prev = nullptr) = 0;

  Properties properties;
};

struct Port : public Node {
  using Node::Node;
  CommunicationGraph::vertex_descriptor vertex;
  Node* neighbour = nullptr;
  std::string data;

  Port* findPort(Node*) override {
    return this;
  }

  bool free() const override {
    return neighbour == nullptr;
  }

  void addNeighbour(Node* node) override {
    assert(free());
    neighbour = node;
  }

  std::optional<Properties> fold(Node* prev = nullptr) override {
    if (prev == nullptr) {
      if (neighbour == nullptr) {
        return std::nullopt;
      }
      else if (auto other = neighbour->fold(this)) {
        return combine(properties, *other);
      }
      else {
        return std::nullopt;
      }
    }
    else {
      return { properties };
    }
  }
};

struct Cable : public Node {
  using Node::Node;
  Node* neighbours[2] {};
  uint8_t neighbourCount = 0;

  Port* findPort(Node* prev = nullptr) override {
    assert(prev != nullptr &&
           (neighbours[0] == prev || neighbours[1] == prev));

    if (neighbours[0] == nullptr || neighbours[1] == nullptr) {
      return nullptr;
    }

    if (prev == neighbours[0]) {
      return neighbours[1]->findPort(this);
    }
    else {
      return neighbours[0]->findPort(this);
    }
  }

  bool free() const override {
    return neighbourCount < 2;
  }

  void addNeighbour(Node *node) override {
    assert(free());
    neighbours[neighbourCount++] = node;
  }

  std::optional<Properties> fold(Node *prev) override {
    assert(prev == nullptr || prev == neighbours[0] || prev == neighbours[1]);

    if (neighbourCount < 2) {
      return std::nullopt;
    }

    auto ra = neighbours[0]->fold(this);
    auto rb = neighbours[1]->fold(this);

    if (prev == nullptr) {
      if (ra && rb) {
        return combine(properties, combine(*ra, *rb));
      } else {
        return std::nullopt;
      }
    }
    else {
      if (prev == neighbours[0]) {
        return combine(properties, *rb);
      }
      else {
        return combine(properties, *ra);
      }
    }
  }
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
    : Component()
  {
    this->port.vertex = this->vertex;
  }

  void update() override {
    this->picture = this->port.data;
    if (this->picture.empty()) {
      this->picture = gen_random(12);
    }
  }

  void render() const override {
    ImGui::Begin("Monitor");
    ImGui::Text("%s", this->picture.c_str());
    ImGui::End();
  }

  Port port { Properties { true, 1.0f } };

private:
  std::string gen_random(int len) {
    std::string s;
    s.reserve(len);

    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
      s.append(1, alphanum[rand() % (sizeof(alphanum) - 1)]);
    }

    return s;
  }
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

  Port port { Properties { true, 1.0f } };
};

void connect(Node* a, Node* b) {
  assert(a != b);
  assert(a->free() && b->free());

  a->addNeighbour(b);
  b->addNeighbour(a);

  Port* left = a->findPort(b);
  Port* right = b->findPort(a);

  if (left && right) {
    Properties result = *left->fold();
    boost::add_edge(left->vertex, right->vertex, boost::property<edge_property_t, EdgeProperties> {
        EdgeProperties { result.picture, result.power, left, right }
    }, G);
  }
}

void connect(Node& a, Node &b) {
  connect(&a, &b);
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

  /* Components */
  Monitor monitor;
  Camera camera;
  Cable cable { Properties { true, 1.0f } };

  std::vector<std::pair<std::string, Node&>> nodes {
          { "Monitor", monitor.port },
          { "Camera", camera.port },
          { "Cable", cable },
  };

  /* Main loop */
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    /* Rendering */
    glClearColor(0.17f, 0.24f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

//    glUseProgram(prog);
//    glBindVertexArray(vao);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
//    glBindVertexArray(0);
//    glUseProgram(0);

    monitor.update();
    camera.update();

    monitor.render();
    camera.render();

    update();

    ImGui::Begin("Nodes");
    for (int i = 0; i < nodes.size(); i++) {
      ImGui::Text("%d: %s", i, nodes[i].first.c_str());
    }
    ImGui::End();

    ImGui::Begin("Console");
    char buf[64];
    bool flag = ImGui::InputText("Command", buf, 64, ImGuiInputTextFlags_EnterReturnsTrue);
    if (flag) {
      char cmd[64];
      int a;
      int b;
      sscanf(buf, "%s %d %d", cmd, &a, &b);

      std::string command(cmd);

      if (a < nodes.size() && b < nodes.size()) {
        if (command == "c") {
          connect(nodes[a].second, nodes[b].second);
        }
      }
    }
    ImGui::End();

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  return 0;
}
