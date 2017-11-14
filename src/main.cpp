#include <optional>

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

#include <Foundation/Wiring.hpp>
#include <Foundation/Component.hpp>

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

  Wiring::Port port;
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
  Wiring::Port port;
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
  Wiring::Port port;
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
  Wiring::Port port;
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

  Wiring::Port port;
};

struct PictureSystem {
  static void update() {
    for (auto ix : boost::make_iterator_range(boost::edges(Wiring::graph()))) {
      auto& prop = Wiring::propertyMap()[ix];
      if (prop.capabilities.picture.enabled) {
        swap(prop.a, prop.b, prop.capabilities.picture.errorRate);
      }
    }
  }

  static void swap(Wiring::Port* a, Wiring::Port* b, float errorRate) {
    if (randomFloat() < errorRate) {
      a->pictureBuffer.pictureData = randomColor();
      b->pictureBuffer.pictureData = randomColor();
    }
    std::swap(a->pictureBuffer, b->pictureBuffer);
  }
};

struct EnergySystem {
  static void update() {
    for (auto ix : boost::make_iterator_range(boost::edges(Wiring::graph()))) {
      auto& prop = Wiring::propertyMap()[ix];
      if (prop.capabilities.energy.enabled) {
        swap(prop.a, prop.b, prop.capabilities.energy.throughput);
      }
    }
  }

  static void swap(Wiring::Port* a, Wiring::Port* b, float throughput) {
    a->energyBuffer.energyPool = std::min(throughput, b->energyBuffer.energyOffer);
    b->energyBuffer.energyOffer = 0.0f;
    b->energyBuffer.energyPool = std::min(throughput, a->energyBuffer.energyOffer);
    a->energyBuffer.energyOffer = 0.0f;
  }
};

struct MessageSystem {
  static void update() {
    for (auto ix : boost::make_iterator_range(boost::edges(Wiring::graph()))) {
      auto& prop = Wiring::propertyMap()[ix];
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
  Wiring::Cable cable1({ { true, 0.1f }, { false, 0.0f }, { true } });
  Wiring::Cable cable2({ { false, 0.0f }, { true, 100.0f }, { true } });
  Wiring::Cable cable3({ { false, 0.0f }, { true, 5.0f }, { true } });

  std::vector<std::pair<std::string, Wiring::Node&>> nodes {
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
            Wiring::connect(nodes[a].second, nodes[b].second);
          }
          if (command == "d") {
            Wiring::disconnect(nodes[a].second, nodes[b].second);
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
