#include <optional>
#include <queue>
#include <regex>
#include <experimental/filesystem>
#include <fstream>

#include <fmt/format.h>
#include <gsl/gsl>

#include <SDL2/SDL.h>
#include <glad.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <Graphics/Program.hpp>
#include <Graphics/Geometry.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Camera.hpp>
#include <Graphics/OrbitControls.hpp>

#include <Util/Random.hpp>

#include <Foundation/Universe.hpp>
#include <Foundation/Infrastructures/Wiring.hpp>
#include <Foundation/Infrastructures/Wireless.hpp>
#include <Foundation/Infrastructures/Manual.hpp>
#include <Foundation/Systems/Video.hpp>
#include <Foundation/Systems/Energy.hpp>
#include <Foundation/Systems/Text.hpp>
#include <Foundation/Components/CPU.hpp>
#include <Foundation/Components/Terminal.hpp>

#include <json.hpp>

class Monitor : public Component {
public:
  explicit Monitor(Universe* w)
    : Component(w)
  {
    this->addPort("video", new Antenna(200.0f, 42.0f, Capabilities {
        .video = { true, 0.0f },
        .energy = { false, 0.0f },
        .text = { false },
    }));

    this->addPort("data", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { false, 0.0f },
        .text = { true },
    }));

    this->addPort("energy", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { false },
    }));

    this->debugger.addCommand("clear", [this] {
      messages.clear();
    });
  }

  void update() override {
    Component::update();

    if (auto data = this->universe->system<VideoSystem>().receive(port("video"))) {
      this->color = *data;
    }
    else {
      this->color = glm::vec3 { 0.0f, 0.0f, 0.0f };
    }

    while (auto msg = this->universe->system<TextSystem>().receive(port("data"))) {
      this->messages.push_back(*msg);
    }
  }

  void render() override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(color.r, color.g, color.b));
    ImGui::SetNextWindowContentSize({ 128, 128 });
    ImGui::Begin("Monitor", nullptr, ImGuiWindowFlags_NoResize);
    for (auto& msg : messages) {
      ImGui::Text("%s", msg.c_str());
    }
    ImGui::End();
    ImGui::PopStyleColor();
  }

  std::string name() const override {
    return "monitor";
  }

  std::string defaultPort() const override {
    return "data";
  }

  glm::vec3 color;
  std::vector<std::string> messages;
};

class Camera : public Component {
public:
  explicit Camera(Universe* w)
    : Component(w)
  {
    addPort("video", new Antenna(100.0f, 40.0f, Capabilities {
        .video = { true, 0.0f },
        .energy = { false, 0.0f },
        .text = { false },
    }));

    addPort("energy", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { false },
    }));

    debugger.addCommand("set_color", [this](float r, float g, float b) {
      color = { r, g, b };
    });

    debugger.addCommand("get_color", [this]() {
      return fmt::format("{} {} {}", color.r, color.g, color.b);
    });

    debugger.addCommand("set_freq", [this](float f) {
      auto* a = dynamic_cast<Antenna*>(port("video"));
      a->frequency = f;
    });

    debugger.addCommand("get_freq", [this]() {
      auto* a = dynamic_cast<Antenna*>(port("video"));
      return fmt::format("{}", a->frequency);
    });
  }

  void update() override {
    Component::update();

    universe->system<VideoSystem>().send(port("video"), color);
  }

  void render() override {
    auto* a = dynamic_cast<Antenna*>(port("video"));

    ImGui::SetNextWindowSize({ 256, 0 });
    ImGui::Begin("Camera");
    ImGui::ColorPicker3("Color", (float*)&color);
    ImGui::SliderFloat("Frequency", &a->frequency, 40.0f, 50.0f);
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

class Generator : public Component {
public:
  explicit Generator(Universe* w)
    : Component(w)
    , history(256, 0)
  {
    addPort("energy", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 100.0f },
        .text = { true }
    }));
  }

  void update() override {
    Component::update();

    noise = (randomFloat() - 0.5f) * 10.0f;
    this->universe->system<EnergySystem>().produce(port("energy"), power + noise);
    history.push_back(power + noise);
  }

  void render() override {
    ImGui::SetNextWindowSize({ 256.0f, 0.0f });
    ImGui::Begin("Generator");

    ImGui::PushItemWidth(-1);
    ImGui::SliderFloat("##power", &power, 0.0f, 100.0f);
    ImGui::PopItemWidth();

    while (history.size() > 256) {
      history.erase(history.begin());
    }
    ImGui::PushItemWidth(-1);
    ImGui::PlotLines("##history", history.data(), history.size(), 0, nullptr, 0.0f, 100.0f);
    ImGui::PopItemWidth();

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

class Lamp : public Component {
public:
  explicit Lamp(Universe* w)
    : Component(w)
    , id { ++counter }
  {
    addPort("energy", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { true },
    }));
  }

  void update() override {
    Component::update();

    float energy = this->universe->system<EnergySystem>().consume(port("energy"), 10.0f);
    satisfaction = energy / 10.0f;
  }

  void render() override {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImU32)ImColor(satisfaction, satisfaction, 0.0f));
    ImGui::SetNextWindowContentSize({ 64, 64 });
    ImGui::Begin(name().c_str(), nullptr, ImGuiWindowFlags_NoResize);
    ImGui::End();
    ImGui::PopStyleColor();
  }

  std::string name() const override {
    return fmt::format("lamp{}", id);
  }

  std::string defaultPort() const override {
    return "energy";
  }

  float satisfaction = 0.0f;

  static inline size_t counter = 0;
  size_t id = 0;
};

class Splitter : public Component {
public:
  explicit Splitter(Universe* u)
    : Component(u)
  {
    addPort("a", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 20.0f },
        .text = { false },
    }));

    addPort("b", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 20.0f },
        .text = { false },
    }));

    addPort("c", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 20.0f },
        .text = { false },
    }));
  }

  void render() override { }

  std::vector<std::pair<float, Endpoint*>> redistributeEnergy(Endpoint* p) override {
    std::vector<std::pair<float, Endpoint*>> neighbours;

    if (p != port("a")) { neighbours.emplace_back(0.5f, port("a")); }
    if (p != port("b")) { neighbours.emplace_back(0.5f, port("b")); }
    if (p != port("c")) { neighbours.emplace_back(0.5f, port("c")); }

    return neighbours;
  }

  std::string name() const override {
    return "splitter";
  }

  std::string defaultPort() const override {
    return "a";
  }
};

class Switch : public Component {
public:
  explicit Switch(Universe* u)
    : Component(u)
  {
    addPort("a", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 20.0f },
        .text = { false },
    }));

    addPort("b", this->universe->infrastructure<Wiring>().createSocket(Capabilities {
        .video = { false, 0.0f },
        .energy = { true, 20.0f },
        .text = { false },
    }));

    debugger.addCommand("toggle", [this] {
      toggle = !toggle;
    });
  }

  void render() override {
    ImGui::Begin("Switch");
    ImGui::Checkbox("Toggle", &toggle);
    ImGui::End();
  }

  std::vector<std::pair<float, Endpoint*>> redistributeEnergy(Endpoint* p) override {
    std::vector<std::pair<float, Endpoint*>> neighbours;

    if (toggle) {
      if (p != port("a")) { neighbours.emplace_back(1.0f, port("a")); }
      if (p != port("b")) { neighbours.emplace_back(1.0f, port("b")); }
    }

    return neighbours;
  }

  std::string name() const override {
    return "switch";
  }

  std::string defaultPort() const override {
    return "a";
  }

  bool toggle = true;
};

class Door : public Component {
public:
  explicit Door(Universe* u)
    : Component(u)
  {
    debugger.addCommand("toggle", [this] { isOpen = !isOpen; });
    debugger.addCommand("open", [this] { isOpen = true; });
    debugger.addCommand("close", [this] { isOpen = false; });
  }

  void render() override { }

  std::string name() const override {
    return "door";
  }

  bool isOpen = false;

  Mesh open = Geometry::load("res/door-open.obj");
  Mesh close = Geometry::load("res/door-closed.obj");
};

void DrawGraph(Universe& universe) {
  static ImVec2 scrolling = ImVec2(0.0f, 0.0f);

  ImColor red   { 1.0f, 0.0f, 0.0f };
  ImColor blue  { 0.0f, 0.0f, 1.0f };
  ImColor green { 0.0f, 1.0f, 0.0f };
  ImColor white { 1.0f, 1.0f, 1.0f };

  ImGui::Begin("Graph");

  ImGui::BeginChild("scrolling_region", { 0, 0 }, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

  auto* window = ImGui::GetCurrentWindow();
  ImVec2 offset = window->DC.CursorPos - scrolling;

  /* Generate component positions */
  // TODO: Save & load this
  for (auto& component : universe.components) {
    if (component->position == glm::vec2 { 0.0f, 0.0f }) {
      glm::vec2 size { window->Size.x, window->Size.y };
      glm::vec2 padding = 0.1f * size;
      component->position = padding + (size - 2.0f * padding) * glm::vec2 { randomFloat(), randomFloat() };

      int i = 0;
      for (auto& pair : component->ports) {
        i++;
        float d = 2.0f * glm::pi<float>() * float(i) / float(component->ports.size());
        Endpoint* p = pair.second.get();
        p->position = glm::vec2 { glm::sin(d), glm::cos(d) } * 12.0f;
      }
    }
  }

  /* Draw connections */
  for (auto& connection : universe.connections) {
    auto fPos = connection.from->globalPosition();
    auto tPos = connection.to->globalPosition();
    auto mPos = fPos + (tPos - fPos) * 0.5f;

    ImColor active { 1.0f, 1.0f, 1.0f, 1.0f };
    ImColor inactive { 1.0f, 1.0f, 1.0f, 0.5f };

    window->DrawList->AddLine(offset + fPos, offset + mPos, active, 1.0f);
    window->DrawList->AddLine(offset + mPos, offset + tPos, inactive, 1.0f);
  }

  static Component* active = nullptr;
  if (!ImGui::IsMouseDown(0)) {
    active = nullptr;
  }

  std::vector<std::pair<glm::vec2, Socket*>> socketPositions;

  /* Draw components */
  for (auto& c : universe.components) {
    window->DrawList->AddCircleFilled(offset + c->position, 4.0f, white);

    ImVec2 labelSize = ImGui::CalcTextSize(c->name().c_str());
    ImVec2 labelPadding { 8.0f, 4.0f };
    ImColor labelColor { 0.0f, 0.0f, 0.0f, 0.5f };
    ImVec2 labelPos = offset + c->position - labelSize * 0.5f - labelPadding + ImVec2 { 0.0f, 32.0f };

    window->DrawList->AddRectFilled(labelPos, labelPos + labelSize + labelPadding * 2.0f, labelColor, 2.0f);
    window->DrawList->AddText(labelPos + labelPadding, white, c->name().c_str());

    /* Draw ports */
    for (auto& pair : c->ports) {
      Endpoint* p = pair.second.get();
      ImVec2 ppos { p->globalPosition().x, p->globalPosition().y };

      window->DrawList->AddCircleFilled(offset + ppos, 2.0f, p->name() == "debug" ? red : white);

      if (auto* a = dynamic_cast<Antenna*>(p)) {
        window->DrawList->AddCircleFilled(offset + ppos, a->radius, ImColor { 1.0f, 1.0f, 1.0f, 0.1f }, 72);
      }

      if (auto* s = dynamic_cast<Socket*>(p)) {
        socketPositions.emplace_back(p->globalPosition(), s);
      }
    }

    ImGui::SetCursorScreenPos(offset + c->position - ImVec2 { 6.0f, 6.0f });
    ImGui::InvisibleButton("##handle", { 12.0f, 12.0f });

    if (ImGui::IsItemClicked(0)) {
      active = c.get();
    }

    if (active == c.get()) {
      c->position += glm::vec2 { ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y };
    }
  }

  /* Draw cables */
  auto& wiring = universe.infrastructure<Wiring>();

  static Connector* activeC = nullptr;
  if (!ImGui::IsMouseDown(0) && activeC != nullptr) {
    activeC->magnetic = false;
    activeC = nullptr;
  }

  for (auto& cable : wiring.cables) {
    ImVec2 aPos = offset + cable->a.position();
    ImVec2 bPos = offset + cable->b.position();

    window->DrawList->AddLine(aPos, bPos, red, 2.0f);
    window->DrawList->AddCircleFilled(aPos, 2.0f, white);

    ImGui::SetCursorScreenPos(aPos - ImVec2 { 3.0f, 3.0f });
    ImGui::InvisibleButton("##a", { 6.0f, 6.0f });
    if (ImGui::IsItemClicked(0)) {
      activeC = &cable->a;
      activeC->magnetic = true;
    }
    if (ImGui::IsItemClicked(1)) {
      if (cable->a.other) {
        wiring.separate(&cable->a, cable->a.other);
      }
    }

    window->DrawList->AddCircleFilled(bPos, 2.0f, white);

    ImGui::SetCursorScreenPos(bPos - ImVec2 { 3.0f, 3.0f });
    ImGui::InvisibleButton("##b", { 6.0f, 6.0f });
    if (ImGui::IsItemClicked(0)) {
      activeC = &cable->b;
      activeC->magnetic = true;
    }
    if (ImGui::IsItemClicked(1)) {
      if (cable->b.other) {
        wiring.separate(&cable->b, cable->b.other);
      }
    }

    if (activeC == &cable->a || activeC == &cable->b) {
      activeC->position() += glm::vec2 { ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y };
    }
  }

  if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f)) {
    scrolling -= ImGui::GetIO().MouseDelta;
  }

  ImGui::EndChild();

  ImGui::End();
}

void SystemUI(Universe& universe) {
  ImGui::Begin("Systems");

  for (auto& system : universe.systems) {
    if (ImGui::TreeNode(system->name.c_str())) {
      system->UI();
      ImGui::TreePop();
    }
  }

  ImGui::End();
}

int main() {
  SDL_Init(SDL_INIT_EVERYTHING);
  auto quitSdl = gsl::finally([] { SDL_Quit(); });

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

  SDL_DisplayMode displayMode {};
  SDL_GetCurrentDisplayMode(0, &displayMode);

  SDL_Window* window = SDL_CreateWindow("",
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        displayMode.w, displayMode.h,
                                        SDL_WINDOW_OPENGL| SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN_DESKTOP);
  auto destroyWindow = gsl::finally([window] { SDL_DestroyWindow(window); });

  SDL_GLContext context = SDL_GL_CreateContext(window);
  auto deleteContext = gsl::finally([context] { SDL_GL_DeleteContext(context); });

  /* Enable VSync */
  SDL_GL_SetSwapInterval(1);

  /* Load OpenGL */
  if(!gladLoadGL()) {
    printf("Something went wrong!\n");
    exit(-1);
  }

  glEnable(GL_DEPTH_TEST);

  /* Initialise ImGui */
  ImGui_ImplSdlGL3_Init(window);
  auto shutdownImgui = gsl::finally([] { ImGui_ImplSdlGL3_Shutdown(); });

  /* Components */
  Universe universe;

  universe.add<Wireless>();
  universe.add<Manual>();
  universe.add<Wiring>();

  universe.add<VideoSystem>();
  universe.add<EnergySystem>();
  universe.add<TextSystem>();

  universe.add<Monitor>();
  universe.add<Camera>();
  universe.add<Generator>();
  universe.add<Lamp>();
  universe.add<Lamp>();
  universe.add<CPU>();
  universe.add<Terminal>();
  universe.add<Splitter>();
  universe.add<Switch>();
  Door* door = universe.add<Door>();

  universe.infrastructure<Manual>().load("connections.json");

  /* Graphics */
  gl::Camera camera { std::make_unique<OrbitControls>(glm::vec3 { 0.0f }, 10.0f) };
  Program program { "shd/basic.vert", "shd/basic.frag" };

  /* Main loop */
  bool done = false;
  while (!done) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        done = true;
      }
      camera.processEvent(e);
      ImGui_ImplSdlGL3_ProcessEvent(&e);
    }

    ImGui_ImplSdlGL3_NewFrame(window);

    /* Rendering */
    glClearColor(0.17f, 0.24f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Game tick */
    universe.tick();
    universe.render();

    DrawGraph(universe);
    SystemUI(universe);

    camera.update();

    /* Draw meshes */
    (door->isOpen ? door->open : door->close).draw(camera, program);

    ImGui::Render();
    SDL_GL_SwapWindow(window);
  }

  universe.infrastructure<Manual>().save("connections.json");

  return 0;
}
