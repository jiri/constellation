#include <optional>
#include <queue>
#include <regex>
#include <experimental/filesystem>
#include <fstream>

#include <fmt/format.h>
#include <gsl/gsl>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/Program.hpp>
#include <Util/Random.hpp>

#include <Foundation/Universe.hpp>
#include <Foundation/Components/CPU.hpp>
#include <Foundation/Systems/Picture.hpp>
#include <Foundation/Systems/Energy.hpp>
#include <Foundation/Systems/Text.hpp>
#include <Foundation/Infrastructures/Capabilities.hpp>
#include <Foundation/Infrastructures/Wireless.hpp>
#include <Foundation/Infrastructures/Debug.hpp>

#include <json.hpp>

struct Monitor : public Component {
  explicit Monitor(Universe* w)
    : Component(w)
  {
    ports.emplace("video", new Port(Capabilities {
        .picture = { true, 0.0f },
        .energy = { false, 0.0f },
        .text = { true },
    }));

    ports.emplace("energy", new Port(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { false },
    }));

    updatePorts();
  }

  void update() override {
    if (auto data = universe->get<PictureSystem>().receive(&port("video"))) {
      color = *data;
    }
    else if (noise) {
      color = 0.5f * randomColor();
    }
    else {
      color = glm::vec3 { 0.0f, 0.0f, 0.0f };
    }

    while (auto msg = this->universe->get<TextSystem>().receive(&port("video"))) {
      message += *msg + "\n";
    }
  }

  void render() override {
    ImGui::Checkbox("Monitor noise", &noise);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(color.r, color.g, color.b));
    ImGui::SetNextWindowContentSize({ 128, 128 });
    ImGui::Begin("Monitor", nullptr, ImGuiWindowFlags_NoResize);
    ImGui::Text("%s", message.c_str());
    ImGui::End();
    ImGui::PopStyleColor();
  }

  std::string name() const override {
    return "monitor";
  }

  std::string defaultPort() const override {
    return "video";
  }

  bool noise = false;
  glm::vec3 color;
  std::string message;
};

struct Camera : public Component {
  explicit Camera(Universe* w)
    : Component(w)
  {
    ports.emplace("video", new Port(Capabilities {
        .picture = { true, 0.0f },
        .energy = { false, 0.0f },
        .text = { false },
    }));

    ports.emplace("energy", new Port(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { false },
    }));

    updatePorts();
  }

  void update() override {
    universe->get<PictureSystem>().send(&port("video"), color);
  }

  void render() override {
    ImGui::SetNextWindowSize({ 256, 256 });
    ImGui::Begin("Camera");
    ImGui::ColorPicker3("Color", (float*)&color);
    ImGui::End();
  }

  std::string name() const override {
    return "camera";
  }

  std::string defaultPort() const override {
    return "video";
  }

  glm::vec3 color;
};

struct Generator : public Component {
  explicit Generator(Universe* w)
    : Component(w)
    , history(256, 0)
  {
    ports.emplace("energy", new Port(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 100.0f },
        .text = { true }
    }));

    updatePorts();
  }

  void update() override {
    noise = (randomFloat() - 0.5f) * 10.0f;
    this->universe->get<EnergySystem>().offer(&port("energy"), power + noise);
    history.push_back(power + noise);
  }

  void render() override {
    ImGui::SetNextWindowSize({ 256.0f, 0.0f });
    ImGui::Begin("Generator");

    ImGui::PushItemWidth(-1);
    ImGui::SliderFloat("##power", &power, 0.0f, 100.0f);

    while (history.size() > 256) {
      history.erase(history.begin());
    }
    ImGui::PushItemWidth(-1);
    ImGui::PlotLines("##history", history.data(), history.size(), 0, nullptr, 0.0f, 100.0f);

    ImGui::End();
  }

  std::string name() const override {
    return "generator";
  }

  std::string defaultPort() const override {
    return "energy";
  }

  float power = 50.0f;
  float noise = 0.0f;
  std::vector<float> history;
};

struct Lamp : public Component {
  explicit Lamp(Universe* w)
    : Component(w)
  {
    ports.emplace("energy", new Port(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { true },
    }));

    updatePorts();
  }

  void update() override {
    float energy = this->universe->get<EnergySystem>().request(&port("energy"), 10.0f);
    satisfaction = energy / 10.0f;
  }

  void render() override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(satisfaction, satisfaction, 0.0f));
    ImGui::SetNextWindowContentSize({ 64, 64 });
    ImGui::Begin("Lamp", nullptr, ImGuiWindowFlags_NoResize);
    ImGui::End();
    ImGui::PopStyleColor();
  }

  std::string name() const override {
    return "lamp";
  }

  std::string defaultPort() const override {
    return "energy";
  }

  float satisfaction = 0.0f;
};

struct Terminal : public Component {
  explicit Terminal(Universe* w)
    : Component(w)
  {
    ports.emplace("output", new Port(Capabilities {
        .picture = { false, 0.0f },
        .energy = { false, 0.0f },
        .text = { true },
    }));

    updatePorts();
  }

  void update() override { }

  void render() override {
    ImGui::Begin("Terminal");
    char buf[256] {};
    if (ImGui::InputText("Text", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
      this->universe->get<TextSystem>().send(&port("output"), buf);
      ImGui::SetKeyboardFocusHere();
    }
    ImGui::End();
  }

  std::string name() const override {
    return "terminal";
  }

  std::string defaultPort() const override {
    return "output";
  }
};

void DrawGraph(Universe& universe) {
  static std::unordered_map<Port*, glm::vec2> portPositions;

  ImColor blue   { 0.0f, 0.0f, 1.0f };
  ImColor green  { 0.0f, 1.0f, 0.0f };
  ImColor white  { 1.0f, 1.0f, 1.0f };

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar
                           | ImGuiWindowFlags_NoResize
                           | ImGuiWindowFlags_NoScrollbar
                           | ImGuiWindowFlags_NoSavedSettings
                           | ImGuiWindowFlags_NoFocusOnAppearing
                           | ImGuiWindowFlags_NoBringToFrontOnFocus;

  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4 { 0.0f, 0.0f, 0.0f, 0.0f });
  ImGui::SetNextWindowSize({ 800.0f, 600.0f });
  ImGui::SetNextWindowPos({ 0.0f, 0.0f });
  ImGui::Begin("", nullptr, flags);

  auto* window = ImGui::GetCurrentWindow();
  ImVec2 offset = window->DC.CursorPos;

  for (auto* component : universe.components) {
    if (component->position == glm::vec2 { 0.0f, 0.0f }) {
      glm::vec2 size { window->Size.x, window->Size.y };
      glm::vec2 padding = 0.1f * size;
      component->position = padding + (size - 2.0f * padding) * glm::vec2 { randomFloat(), randomFloat() };

      int i = 0;
      for (auto& pair : component->ports) {
        i++;
        float d = 2.0f * glm::pi<float>() * float(i) / float(component->ports.size());
        Port* p = pair.second.get();
        portPositions[p] = glm::vec2 { glm::sin(d), glm::cos(d) } * 12.0f;
      }
    }
  }

  for (auto& connection : universe.connections) {
    auto aPos = connection.a->component->position + portPositions[connection.a];
    auto bPos = connection.b->component->position + portPositions[connection.b];

    float thickness = 0.0f;

    window->DrawList->ChannelsSplit(3);

    if (connection.capabilities.energy.enabled) {
      thickness += 2.0f;
      window->DrawList->ChannelsSetCurrent(2);
      window->DrawList->AddLine(offset + ImVec2 { aPos.x, aPos.y },
                                offset + ImVec2 { bPos.x, bPos.y }, blue, thickness);
    }

    if (connection.capabilities.picture.enabled || connection.capabilities.text.enabled) {
      thickness += 2.0f;
      window->DrawList->ChannelsSetCurrent(1);
      window->DrawList->AddLine(offset + ImVec2 { aPos.x, aPos.y },
                                offset + ImVec2 { bPos.x, bPos.y }, green, thickness);
    }

    window->DrawList->ChannelsSetCurrent(0);
    window->DrawList->AddLine(offset + ImVec2 { aPos.x, aPos.y },
                              offset + ImVec2 { bPos.x, bPos.y }, white, 1.0f);

    window->DrawList->ChannelsMerge();
    
  }

  static Component* active = nullptr;
  for (auto* c : universe.components) {
    auto pos = offset + ImVec2 { c->position.x, c->position.y };
    window->DrawList->AddCircleFilled(pos, 4.0f, white);
    window->DrawList->AddCircleFilled(pos, 50.0f, ImColor { 1.0f, 1.0f, 1.0f, 0.1f });
    window->DrawList->AddText(pos + ImVec2 { 8.0f, -7.0f }, white, c->name().c_str());

    for (auto& pair : c->ports) {
      Port* p = pair.second.get();
      ImVec2 ppos { portPositions[p].x, portPositions[p].y };
      window->DrawList->AddCircleFilled(pos + ppos, 2.0f, white);
    }

    ImGui::SetCursorScreenPos(pos - ImVec2 { 6.0f, 6.0f });
    ImGui::InvisibleButton("##handle", { 12.0f, 12.0f });

    if (ImGui::IsItemClicked(0)) {
      active = c;
    }
    if (!ImGui::IsMouseDown(0)) {
      active = nullptr;
    }

    if (active == c) {
      c->position += glm::vec2 { ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y };
    }
  }

  ImGui::End();
  ImGui::PopStyleColor();
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
  glfwSwapInterval(1);

  /* Initialise ImGui */
  ImGui_ImplGlfwGL3_Init(window, true);
  auto shutdownImgui = gsl::finally([] { ImGui_ImplGlfwGL3_Shutdown(); });

  /* Components */
  Universe universe {
      {
          new Monitor { &universe },
          new Camera { &universe },
          new Generator { &universe },
          new Lamp { &universe },
          new CPU { &universe },
          new Terminal { &universe },
      },
      {
          new PictureSystem { &universe },
          new EnergySystem { &universe },
          new TextSystem { &universe },
      },
      {
//          new Wireless { &universe },
          new Debug { &universe },
      },
  };

  universe.load("connections.json");

  /* Main loop */
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    /* Rendering */
    glClearColor(0.17f, 0.24f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Game tick */
    universe.tick();

    DrawGraph(universe);

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  universe.save("connections.json");

  return 0;
}
