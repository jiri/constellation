#include <optional>

#include <boost/graph/adjacency_list.hpp>
#include <fmt/format.h>
#include <gsl/gsl>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/Program.hpp>
#include <Util/Random.hpp>

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
  virtual void removeNeighbour(Node* node) = 0;
  virtual bool isNeighbourOf(Node* node) const = 0;
  virtual std::optional<Properties> fold(Node* prev = nullptr) = 0;

  Properties properties;
};

struct Port : public Node {
  using Node::Node;
  CommunicationGraph::vertex_descriptor vertex;
  Node* neighbour = nullptr;
  glm::vec3 data;

  Port* findPort(Node*) override {
    return this;
  }

  bool free() const override {
    return neighbour == nullptr;
  }

  bool connected() {
    return neighbour && neighbour->findPort(this);
  }

  void addNeighbour(Node* node) override {
    assert(free());
    neighbour = node;
  }

  void removeNeighbour(Node* node) override {
    assert(neighbour == node);
    neighbour = nullptr;
  }

  bool isNeighbourOf(Node* node) const override {
    return neighbour == node;
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

  void removeNeighbour(Node* node) override {
    assert(neighbours[0] == node || neighbours[1] == node);

    if (neighbours[1] == node) {
      neighbours[1] = nullptr;
      neighbourCount--;
    }
    else {
      neighbours[0] = neighbours[1];
      neighbours[1] = nullptr;
      neighbourCount--;
    }
  }

  bool isNeighbourOf(Node* node) const override {
    return neighbours[0] == node || neighbours[1] == node;
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
  glm::vec3 color;

  Monitor()
    : Component()
  {
    this->port.vertex = this->vertex;
  }

  void update() override {

    if (this->port.connected()) {
      color = port.data;
    } else {
      color = 0.5f * randomColor();
    }
  }

  void render() const override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(color.r, color.g, color.b));
    ImGui::SetNextWindowContentSize({ 128, 128 });
    ImGui::Begin("Monitor");
    ImGui::End();
    ImGui::PopStyleColor();
  }

  Port port { Properties { true, 1.0f } };
};

struct Camera : public Component {
  Camera()
      : Component() {
    this->port.vertex = this->vertex;
  }

  void update() override {
    this->port.data = color;
  }

  void render() const override {
    ImGui::SetNextWindowSize({ 256, 256 });
    ImGui::Begin("Camera");
    ImGui::ColorPicker3("Color", (float*)&color);
    ImGui::End();
  }

  glm::vec3 color;
  Port port { Properties { true, 1.0f } };
};

void connect(Node* a, Node* b) {
  if (a->isNeighbourOf(b) && b->isNeighbourOf(a)) {
    return;
  }

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

void disconnect(Node* a, Node* b) {
  assert(a != b);
  assert(a->isNeighbourOf(b) && b->isNeighbourOf(a));

  Port* left = a->findPort(b);
  Port* right = b->findPort(a);

  if (left && right) {
    auto edge = boost::edge(left->vertex, right->vertex, G);
    assert(edge.second);
    boost::remove_edge(edge.first, G);
  }

  a->removeNeighbour(b);
  b->removeNeighbour(a);
}

void connect(Node& a, Node &b) {
  connect(&a, &b);
}

void disconnect(Node& a, Node&b ) {
  disconnect(&a, &b);
}

struct PictureSystem {
  static void update() {
    boost::property_map<CommunicationGraph, edge_property_t>::type EdgePropertyMap = boost::get(edge_property_t(), G);

    for (auto edgePair = boost::edges(G); edgePair.first != edgePair.second; ++edgePair.first) {
      auto& prop = EdgePropertyMap[*edgePair.first];
      if (prop.picture) {
        swap(prop.a, prop.b);
      }
    }
  }

  static void swap(Port* a, Port* b) {
    std::swap(a->data, b->data);
  }
};

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

    monitor.update();
    camera.update();

    monitor.render();
    camera.render();

    PictureSystem::update();

    ImGui::Begin("Nodes");
    for (int i = 0; i < nodes.size(); i++) {
      ImGui::Text("%d: %s", i, nodes[i].first.c_str());
    }
    ImGui::End();

    ImGui::Begin("Console");
    char buf[64];
    bool flag = ImGui::InputText("Command", buf, 64, ImGuiInputTextFlags_EnterReturnsTrue);
    if (ImGui::IsRootWindowOrAnyChildFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
      ImGui::SetKeyboardFocusHere(0);
    if (flag) {
      char cmd[64];
      int a;
      int b;
      int r = sscanf(buf, "%s %d %d", cmd, &a, &b);

      if (r == 3) {
        std::string command(cmd);

        if (a < nodes.size() && b < nodes.size()) {
          if (command == "c") {
            connect(nodes[a].second, nodes[b].second);
          }
          if (command == "d") {
            disconnect(nodes[a].second, nodes[b].second);
          }
        }
      }
    }
    ImGui::End();

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  return 0;
}
