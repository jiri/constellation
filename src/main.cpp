#include <optional>
#include <queue>

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

#include <Foundation/Wiring.hpp>
#include <Foundation/Component.hpp>
#include <Foundation/Components/CPU.hpp>

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

  void render() override {
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

  void render() override {
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

  void render() override {
    ImGui::Begin("Generator");
    ImGui::SliderFloat("Power", &power, 0.0f, 100.0f);
    ImGui::End();
  }

  float power = 50.0f;
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

  void render() override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(satisfaction, satisfaction, 0.0f));
    ImGui::SetNextWindowContentSize({ 64, 64 });
    ImGui::Begin("Lamp", nullptr, ImGuiWindowFlags_NoResize);
    ImGui::End();
    ImGui::PopStyleColor();
  }

  float satisfaction = 0.0f;
  Wiring::Port port;
};

struct Terminal : public Component {
  Terminal()
    : Component()
    , port(this, { { false, 0.0f }, { false, 0.0f }, { true } })
  { }

  void update() override { }

  void render() override {
    ImGui::Begin("Terminal");
    char buf[256] {};
    if (ImGui::InputText("Text", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
      port.textBuffer.send(buf);
    }
    ImGui::End();
  }

  Wiring::Port port;
};

struct System {
  virtual ~System() = default;

  virtual bool filter(const Wiring::Edge& edge) const = 0;
  virtual void swap(Wiring::Edge& edge) = 0;

  void update() {
    for (auto& t : Wiring::graph().edges) {
      auto& edge = std::get<2>(t);
      if (this->filter(edge)) {
        this->swap(edge);
      }
    }
  }
};

struct PictureSystem : public System{
  bool filter(const Wiring::Edge& edge) const override {
    return edge.capabilities.picture.enabled;
  }

  void swap(Wiring::Edge& edge) override {
    if (randomFloat() < edge.capabilities.picture.errorRate) {
      edge.a->pictureBuffer.pictureData = randomColor();
      edge.b->pictureBuffer.pictureData = randomColor();
    }
    std::swap(edge.a->pictureBuffer, edge.b->pictureBuffer);
  }
};

struct EnergySystem : public System {
  bool filter(const Wiring::Edge& edge) const override {
    return edge.capabilities.energy.enabled;
  }

  void swap(Wiring::Edge& edge) override {
    auto t = edge.capabilities.energy.throughput;
    edge.a->energyBuffer.energyPool = std::min(t, edge.b->energyBuffer.energyOffer);
    edge.b->energyBuffer.energyOffer = 0.0f;
    edge.b->energyBuffer.energyPool = std::min(t, edge.a->energyBuffer.energyOffer);
    edge.a->energyBuffer.energyOffer = 0.0f;
  }
};

struct MessageSystem : public System {
  bool filter(const Wiring::Edge& edge) const override {
    return edge.capabilities.text.enabled;
  }

  void swap(Wiring::Edge& edge) override {
    std::swap(edge.a->textBuffer, edge.b->textBuffer);
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
  Terminal terminal;
  Wiring::Cable cable1({ { true, 0.1f }, { false, 0.0f }, { true } });
  Wiring::Cable cable2({ { false, 0.0f }, { true, 100.0f }, { true } });
  Wiring::Cable cable3({ { false, 0.0f }, { true, 5.0f }, { true } });

  std::vector<std::pair<std::string, Wiring::Node&>> nodes {
          { "Monitor", monitor.port },
          { "Camera", camera.port },
          { "Generator", generator.port },
          { "Lamp", lamp.port },
          { "CPU IN", cpu.inPort },
          { "CPU OUT", cpu.outPort },
          { "Terminal", terminal.port },
          { "Cable P", cable1 },
          { "Cable S", cable2 },
          { "Cable W", cable3 },
  };

  std::vector<std::unique_ptr<System>> systems;
  systems.emplace_back(new PictureSystem);
  systems.emplace_back(new EnergySystem);
  systems.emplace_back(new MessageSystem);

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
    terminal.update();

    for (auto& system : systems) {
      system->update();
    }

    monitor.render();
    camera.render();
    generator.render();
    lamp.render();
    cpu.render();
    terminal.render();

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
