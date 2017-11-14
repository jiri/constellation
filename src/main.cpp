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
#include <queue>

#include <Foundation/Systems/Picture.hpp>
#include <Foundation/Systems/Energy.hpp>
#include <Foundation/Systems/Text.hpp>

struct Capabilities {
  Picture::Capability picture;
  Energy::Capability energy;
  Text::Capability text;

  static Capabilities combine(const Capabilities& a, const Capabilities& b) {
    return {
        Picture::Capability::combine(a.picture, b.picture),
        Energy::Capability::combine(a.energy, b.energy),
        Text::Capability::combine(a.text, b.text),
    };
  }
};

struct Component;

struct Vertex {
  Component* component = nullptr;
};

struct vertex_property_t {
  typedef boost::edge_property_tag kind;
};

struct Port;

struct Edge {
  Port* a;
  Port* b;
  Capabilities capabilities;
};

struct edge_property_t {
  typedef boost::edge_property_tag kind;
};

typedef boost::adjacency_list<boost::vecS,
    boost::vecS,
    boost::undirectedS,
    boost::property<vertex_property_t, Vertex>,
    boost::property<edge_property_t, Edge>> CommunicationGraph;
static CommunicationGraph G;

struct Node {
  explicit Node(Capabilities c)
    : capabilities(c)
  { }

  virtual ~Node() = default;

  virtual Port* findPort(Node* prev) = 0;
  virtual bool isFree() const = 0;
  virtual void addNeighbour(Node* node) = 0;
  virtual void removeNeighbour(Node* node) = 0;
  virtual bool isNeighbourOf(Node* node) const = 0;
  virtual std::optional<Capabilities> fold(Node* prev) = 0;

  Capabilities capabilities;
};

struct Port : public Node {
  Port(Component* c, Capabilities cap)
    : Node(cap)
    , component(c)
  { }

  Port* findPort(Node*) override {
    return this;
  }
  bool isFree() const override {
    return neighbour == nullptr;
  }
  void addNeighbour(Node* node) override {
    assert(isFree());
    neighbour = node;
  }
  void removeNeighbour(Node* node) override {
    assert(neighbour == node);
    neighbour = nullptr;
  }
  bool isNeighbourOf(Node* node) const override {
    return neighbour == node;
  }
  std::optional<Capabilities> fold(Node* prev) override {
    if (prev == nullptr) {
      if (neighbour == nullptr) {
        return std::nullopt;
      }
      else if (auto other = neighbour->fold(this)) {
        return Capabilities::combine(capabilities, *other);
      }
      else {
        return std::nullopt;
      }
    }
    else {
      return { capabilities };
    }
  }

  bool connected() {
    return neighbour && neighbour->findPort(this);
  }

  Component* component = nullptr;
  Node* neighbour = nullptr;

  Picture::Buffer pictureBuffer;
  Energy::Buffer energyBuffer;
  Text::Buffer textBuffer;
};

struct Cable : public Node {
  using Node::Node;

  Port* findPort(Node* prev) override {
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
  bool isFree() const override {
    return neighbourCount < 2;
  }
  void addNeighbour(Node *node) override {
    assert(isFree());
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
  std::optional<Capabilities> fold(Node *prev) override {
    assert(prev == nullptr || prev == neighbours[0] || prev == neighbours[1]);

    if (neighbourCount < 2) {
      return std::nullopt;
    }

    auto ra = neighbours[0]->fold(this);
    auto rb = neighbours[1]->fold(this);

    if (prev == nullptr) {
      if (ra && rb) {
        return Capabilities::combine(capabilities, Capabilities::combine(*ra, *rb));
      } else {
        return std::nullopt;
      }
    }
    else {
      if (prev == neighbours[0]) {
        return Capabilities::combine(capabilities, *rb);
      }
      else {
        return Capabilities::combine(capabilities, *ra);
      }
    }
  }

  Node* neighbours[2] {};
  uint8_t neighbourCount = 0;
};

struct Component {
  Component()
    : vertex(boost::add_vertex(G))
  { }

  virtual ~Component() {
    boost::remove_vertex(this->vertex, G);
  }

  virtual void update() = 0;
  virtual void render() const = 0;

  CommunicationGraph::vertex_descriptor vertex;
};

struct Monitor : public Component {
  glm::vec3 color;
  std::string message;

  Monitor()
    : Component()
    , port(this, Capabilities { { true, 0.0f }, { false, 0.0f }, { true } })
  { }

  void update() override {
    if (auto data = port.pictureBuffer.receive()) {
      color = *data;
    }
    else {
      color = 0.5f * randomColor();
    }

    while (auto msg = port.textBuffer.receive()) {
      message += *msg + "\n";
    }
  }

  void render() const override {
    static bool noise = true;
    ImGui::Checkbox("Monitor noise", &noise);
    if (noise) {
      ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(color.r, color.g, color.b));
    }
    ImGui::SetNextWindowContentSize({ 128, 128 });
    ImGui::Begin("Monitor", nullptr, ImGuiWindowFlags_NoResize);
    ImGui::Text("%s", message.c_str());
    ImGui::End();
    if (noise) {
      ImGui::PopStyleColor();
    }
  }

  Port port;
};

struct Camera : public Component {
  Camera()
    : Component()
    , port(this, { { true, 0.0f }, { false, 0.0f } })
  { }

  void update() override {
    this->port.pictureBuffer.send(color);
  }

  void render() const override {
    ImGui::SetNextWindowSize({ 256, 256 });
    ImGui::Begin("Camera");
    ImGui::ColorPicker3("Color", (float*)&color);
    ImGui::End();
  }

  glm::vec3 color;
  Port port;
};

struct Generator : public Component {
  Generator()
    : Component()
    , port(this, { { false, 0.0f }, { true, 100.0f } })
  { }

  void update() override {
    this->port.energyBuffer.offer(power);
  }

  void render() const override {
    ImGui::Begin("Generator");
    ImGui::SliderFloat("Power", &power, 0.0f, 100.0f);
    ImGui::End();
  }

  mutable float power = 50.0f;
  Port port;
};

struct Lamp : public Component {
  Lamp()
    : Component()
    , port(this, { { false, 0.0f }, { true, 10.0f } })
  { }

  void update() override {
    float energy = port.energyBuffer.request(10.0f);
    satisfaction = energy / 10.0f;
  }

  void render() const override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(satisfaction, satisfaction, 0.0f));
    ImGui::SetNextWindowContentSize({ 64, 64 });
    ImGui::Begin("Lamp", nullptr, ImGuiWindowFlags_NoResize);
    ImGui::End();
    ImGui::PopStyleColor();
  }

  float satisfaction = 0.0f;
  Port port;
};

struct CPU : public Component {
  CPU()
    : Component()
    , port(this, { { false, 0.0f }, { true, 10.0f }, { true } })
  { }

  std::stack<int> stack;

  int pop() {
    int x = stack.top();
    stack.pop();
    return x;
  }

  void run(const std::string& program) {
    std::istringstream iss(program);
    std::string word;

    while (iss >> word) {
      if (word == "push") {
        int i;
        iss >> i;
        stack.push(i);
      }
      if (word == "pop") {
        pop();
      }
      if (word == "add") {
        int a = pop();
        int b = pop();
        stack.push(a + b);
      }
      if (word == "neg") {
        stack.push(-pop());
      }
      if (word == "mul") {
        int a = pop();
        int b = pop();
        stack.push(a * b);
      }
      if (word == "divmod") {
        int a = pop();
        int b = pop();
        stack.push(b / a);
        stack.push(b % a);
      }
      if (word == "write") {
        port.textBuffer.send(fmt::format("{}", stack.top()));
      }
    }
  }

  void update() override { }
  void render() const override { }

  Port port;
};


void connect(Node* a, Node* b) {
  if (a->isNeighbourOf(b) && b->isNeighbourOf(a)) {
    return;
  }

  assert(a != b);
  assert(a->isFree() && b->isFree());

  a->addNeighbour(b);
  b->addNeighbour(a);

  Port* left = a->findPort(b);
  Port* right = b->findPort(a);

  if (left && right) {
    Capabilities result = *left->fold(nullptr);
    boost::add_edge(left->component->vertex, right->component->vertex, boost::property<edge_property_t, Edge> {
        Edge { left, right, result }
    }, G);
  }
}

void disconnect(Node* a, Node* b) {
  assert(a != b);
  assert(a->isNeighbourOf(b) == b->isNeighbourOf(a));

  if (!a->isNeighbourOf(b) || !b->isNeighbourOf(a)) {
    return;
  }

  Port* left = a->findPort(b);
  Port* right = b->findPort(a);

  if (left && right) {
    auto edge = boost::edge(left->component->vertex, right->component->vertex, G);
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
      if (prop.capabilities.picture.enabled) {
        swap(prop.a, prop.b, prop.capabilities.picture.errorRate);
      }
    }
  }

  static void swap(Port* a, Port* b, float errorRate) {
    if (randomFloat() < errorRate) {
      a->pictureBuffer.pictureData = randomColor();
      b->pictureBuffer.pictureData = randomColor();
    }
    std::swap(a->pictureBuffer, b->pictureBuffer);
  }
};

struct EnergySystem {
  static void update() {
    boost::property_map<CommunicationGraph, edge_property_t>::type EdgePropertyMap = boost::get(edge_property_t(), G);

    for (auto edgePair = boost::edges(G); edgePair.first != edgePair.second; ++edgePair.first) {
      auto& prop = EdgePropertyMap[*edgePair.first];
      if (prop.capabilities.energy.enabled) {
        swap(prop.a, prop.b, prop.capabilities.energy.throughput);
      }
    }
  }

  static void swap(Port* a, Port* b, float throughput) {
    a->energyBuffer.energyPool = std::min(throughput, b->energyBuffer.energyOffer);
    b->energyBuffer.energyOffer = 0.0f;
    b->energyBuffer.energyPool = std::min(throughput, a->energyBuffer.energyOffer);
    a->energyBuffer.energyOffer = 0.0f;
  }
};

struct MessageSystem {
  static void update() {
    boost::property_map<CommunicationGraph, edge_property_t>::type EdgePropertyMap = boost::get(edge_property_t(), G);

    for (auto edgePair = boost::edges(G); edgePair.first != edgePair.second; ++edgePair.first) {
      auto& prop = EdgePropertyMap[*edgePair.first];
      if (prop.capabilities.text.enabled) {
        std::swap(prop.a->textBuffer, prop.b->textBuffer);
      }
    }
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
  Generator generator;
  Lamp lamp;
  CPU cpu;
  Cable cable1({ { true, 0.1f }, { false, 0.0f }, { true } });
  Cable cable2({ { false, 0.0f }, { true, 100.0f }, { true } });
  Cable cable3({ { false, 0.0f }, { true, 5.0f }, { true } });

  std::vector<std::pair<std::string, Node&>> nodes {
          { "Monitor", monitor.port },
          { "Camera", camera.port },
          { "Generator", generator.port },
          { "Lamp", lamp.port },
          { "CPU", cpu.port },
          { "Cable P", cable1 },
          { "Cable S", cable2 },
          { "Cable W", cable3 },
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
    generator.update();
    lamp.update();
    cpu.update();

    PictureSystem::update();
    EnergySystem::update();
    MessageSystem::update();

    monitor.render();
    camera.render();
    generator.render();
    lamp.render();
    cpu.render();

    ImGui::Begin("Terminal");
    char cbuf[512];
    ImGui::InputTextMultiline("Code", cbuf, 512);
    if (ImGui::Button("Run")) {
      while (cpu.stack.size())
        cpu.stack.pop();
      cpu.run(cbuf);
    }
    ImGui::End();

    ImGui::Begin("Nodes");
    for (int i = 0; i < nodes.size(); i++) {
      ImGui::Text("%d: %s", i, nodes[i].first.c_str());
    }
    ImGui::End();

    ImGui::Begin("Console");
    char buf[64] {};
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
