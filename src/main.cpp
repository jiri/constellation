#include <optional>
#include <queue>
#include <regex>
#include <experimental/filesystem>
#include <fstream>
#include <list>

#include <fmt/format.h>
#include <gsl/gsl>

#include <SDL2/SDL.h>
#include <glad.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_sdl_gl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Graphics/Program.hpp>
#include <Util/Random.hpp>

#include <Foundation/Universe.hpp>
#include <Foundation/Debugger.hpp>
#include <Foundation/Components/CPU.hpp>
#include <Foundation/Systems/Picture.hpp>
#include <Foundation/Systems/Energy.hpp>
#include <Foundation/Systems/Text.hpp>
#include <Foundation/Infrastructures/Capabilities.hpp>
#include <Foundation/Infrastructures/Wiring.hpp>
#include <Foundation/Infrastructures/Wireless.hpp>
#include <Foundation/Infrastructures/Manual.hpp>

#include <json.hpp>

struct Monitor : public Component {
  explicit Monitor(Universe* w)
    : Component(w)
  {
    addPort("video", new Antenna(200.0f, 42.0f, Capabilities {
        .picture = { true, 0.0f },
        .energy = { false, 0.0f },
        .text = { false },
    }));

    addPort("data", new Socket(Capabilities {
        .picture = { false, 0.0f },
        .energy = { false, 0.0f },
        .text = { true },
    }));

    addPort("energy", new Socket(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { false },
    }));

    debugger.addCommand("clear", [this] {
      message = "";
    });
  }

  void update() override {
    Component::update();

    if (auto data = universe->get<VideoSystem>().receive(port("video"))) {
      color = *data;
    }
    else {
      color = glm::vec3 { 0.0f, 0.0f, 0.0f };
    }

    while (auto msg = this->universe->get<TextSystem>().receive(port("data"))) {
      message += *msg + "\n";
    }
  }

  void render() override {
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
    return "data";
  }

  glm::vec3 color;
  std::string message;
};

struct Camera : public Component {
  explicit Camera(Universe* w)
    : Component(w)
  {
    addPort("video", new Antenna(100.0f, 40.0f, Capabilities {
        .picture = { true, 0.0f },
        .energy = { false, 0.0f },
        .text = { false },
    }));

    addPort("energy", new Socket(Capabilities {
        .picture = { false, 0.0f },
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
      auto* a = dynamic_cast<Antenna*>(ports.at("video").get());
      a->frequency = f;
    });

    debugger.addCommand("get_freq", [this]() {
      auto* a = dynamic_cast<Antenna*>(ports.at("video").get());
      return fmt::format("{}", a->frequency);
    });
  }

  void update() override {
    Component::update();

    universe->get<VideoSystem>().send(port("video"), color);
  }

  void render() override {
    auto* a = dynamic_cast<Antenna*>(ports.at("video").get());

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

struct Generator : public Component {
  explicit Generator(Universe* w)
    : Component(w)
    , history(256, 0)
  {
    addPort("energy", new Socket(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 100.0f },
        .text = { true }
    }));
  }

  void update() override {
    Component::update();

    noise = (randomFloat() - 0.5f) * 10.0f;
    this->universe->get<EnergySystem>().produce(port("energy"), power + noise);
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

struct Lamp : public Component {
  explicit Lamp(Universe* w)
    : Component(w)
    , id { ++counter }
  {
    addPort("energy", new Socket(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 10.0f },
        .text = { true },
    }));
  }

  void update() override {
    Component::update();

    float energy = this->universe->get<EnergySystem>().consume(port("energy"), 10.0f);
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

  static size_t counter;
  size_t id = 0;
};

size_t Lamp::counter = 0;

struct Terminal : public Component {
  using Component::Component;

  void update() override {
    while (auto s = this->universe->get<TextSystem>().receive(port("debug"))) {
      messages.push_back(*s);
      newMessage = true;
    }

    Component::update();
  }

  void render() override {
    ImGui::Begin("Terminal");

    ImGui::BeginChild("##text", { 0, -ImGui::GetItemsLineHeightWithSpacing() });
    for (auto& msg : messages) {
      ImGui::Text("%s", msg.c_str());
    }
    if (newMessage) {
      ImGui::SetScrollHere();
      newMessage = false;
    }
    ImGui::EndChild();

    ImGui::PushItemWidth(-1);
    if (ImGui::InputText("##input", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
      this->messages.push_back(fmt::format("> {}", buf));
      this->universe->get<TextSystem>().send(port("debug"), buf);
      ImGui::SetKeyboardFocusHere();
      buf[0] = '\0';
    }
    ImGui::PopItemWidth();

    ImGui::End();
  }

  std::string name() const override {
    return "terminal";
  }

  std::string defaultPort() const override {
    return "port";
  }

  bool newMessage = false;
  char buf[256] {};
  std::list<std::string> messages;
};

class Splitter : public Component {
public:
  explicit Splitter(Universe* u)
    : Component(u)
  {
    addPort("a", new Socket(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 20.0f },
        .text = { false },
    }));

    addPort("b", new Socket(Capabilities {
            .picture = { false, 0.0f },
            .energy = { true, 20.0f },
            .text = { false },
    }));

    addPort("c", new Socket(Capabilities {
            .picture = { false, 0.0f },
            .energy = { true, 20.0f },
            .text = { false },
    }));
  }

  void render() override { }

  std::vector<std::pair<float, Port*>> redistributeEnergy(Port* p) override {
    std::vector<std::pair<float, Port*>> neighbours;

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

struct Switch : public Component {
public:
  explicit Switch(Universe* u)
    : Component(u)
  {
    addPort("a", new Socket(Capabilities {
        .picture = { false, 0.0f },
        .energy = { true, 20.0f },
        .text = { false },
    }));

    addPort("b", new Socket(Capabilities {
        .picture = { false, 0.0f },
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

  std::vector<std::pair<float, Port*>> redistributeEnergy(Port* p) override {
    std::vector<std::pair<float, Port*>> neighbours;

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

    window->DrawList->AddLine(offset + ImVec2 { fPos.x, fPos.y },
                              offset + ImVec2 { mPos.x, mPos.y }, active, 1.0f);
    window->DrawList->AddLine(offset + ImVec2 { mPos.x, mPos.y },
                              offset + ImVec2 { tPos.x, tPos.y }, inactive, 1.0f);
  }

  static Component* active = nullptr;
  if (!ImGui::IsMouseDown(0)) {
    active = nullptr;
  }

  std::vector<std::pair<glm::vec2, Socket*>> socketPositions;

  /* Draw components */
  for (auto* c : universe.components) {
    auto pos = ImVec2 { c->position.x, c->position.y };
    window->DrawList->AddCircleFilled(offset + pos, 4.0f, white);

    ImVec2 labelSize = ImGui::CalcTextSize(c->name().c_str());
    ImVec2 labelPadding { 8.0f, 4.0f };
    ImColor labelColor { 0.0f, 0.0f, 0.0f, 0.5f };
    ImVec2 labelPos = offset + pos - labelSize * 0.5f - labelPadding + ImVec2 { 0.0f, 32.0f };

    window->DrawList->AddRectFilled(labelPos, labelPos + labelSize + labelPadding * 2.0f, labelColor, 2.0f);
    window->DrawList->AddText(labelPos + labelPadding, white, c->name().c_str());

    /* Draw ports */
    for (auto& pair : c->ports) {
      Port* p = pair.second.get();
      ImVec2 ppos { p->globalPosition().x, p->globalPosition().y };

      window->DrawList->AddCircleFilled(offset + ppos, 2.0f, p->name() == "debug" ? red : white);

      if (auto* a = dynamic_cast<Antenna*>(p)) {
        window->DrawList->AddCircleFilled(offset + ppos, a->radius, ImColor { 1.0f, 1.0f, 1.0f, 0.1f }, 72);
      }

      if (auto* s = dynamic_cast<Socket*>(p)) {
        socketPositions.emplace_back(p->globalPosition(), s);
      }
    }

    ImGui::SetCursorScreenPos(offset + pos - ImVec2 { 6.0f, 6.0f });
    ImGui::InvisibleButton("##handle", { 12.0f, 12.0f });

    if (ImGui::IsItemClicked(0)) {
      active = c;
    }

    if (active == c) {
      c->position += glm::vec2 { ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y };
    }
  }

  /* Draw cables */
  auto& wiring = universe.infrastructure<Wiring>();

  static Connector* activeC = nullptr;
  if (!ImGui::IsMouseDown(0)) {
    activeC = nullptr;
  }

  for (auto& cable : wiring.cables) {
    ImVec2 aPos = offset + ImVec2 { cable->a.position.x, cable->a.position.y };
    ImVec2 bPos = offset + ImVec2 { cable->b.position.x, cable->b.position.y };

    window->DrawList->AddLine(aPos, bPos, red, 2.0f);
    window->DrawList->AddCircleFilled(aPos, 2.0f, white);

    ImGui::SetCursorScreenPos(aPos - ImVec2 { 3.0f, 3.0f });
    ImGui::InvisibleButton("##a", { 6.0f, 6.0f });
    if (ImGui::IsItemClicked(0)) {
      activeC = &cable->a;
    }
    if (ImGui::IsItemClicked(1)) {
      cable->a.position -= glm::vec2 { 0.0f, 20.0f };
    }

    window->DrawList->AddCircleFilled(bPos, 2.0f, white);

    ImGui::SetCursorScreenPos(bPos - ImVec2 { 3.0f, 3.0f });
    ImGui::InvisibleButton("##b", { 6.0f, 6.0f });
    if (ImGui::IsItemClicked(0)) {
      activeC = &cable->b;
    }
    if (ImGui::IsItemClicked(1)) {
      cable->b.position -= glm::vec2 { 0.0f, 20.0f };
    }

    if (activeC == &cable->a) {
      cable->a.position += glm::vec2 { ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y };

      for (auto& pair : socketPositions) {
        if (glm::distance(cable->a.position, pair.first) < 4.0f) {
          wiring.join(&cable->a, pair.second);
        }
      }
    }
    if (activeC == &cable->b) {
      cable->b.position += glm::vec2 { ImGui::GetIO().MouseDelta.x, ImGui::GetIO().MouseDelta.y };

      for (auto& pair : socketPositions) {
        if (glm::distance(cable->b.position, pair.first) < 4.0f) {
          wiring.join(&cable->b, pair.second);
        }
      }
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

  for (System* system : universe.systems) {
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

  SDL_Window* window = SDL_CreateWindow("",
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        800, 600, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  if(!gladLoadGL()) {
    printf("Something went wrong!\n");
    exit(-1);
  }

  /* Enable VSync */
  SDL_GL_SetSwapInterval(1);

  /* Initialise ImGui */
  ImGui_ImplSdlGL3_Init(window);
  auto shutdownImgui = gsl::finally([] { ImGui_ImplSdlGL3_Shutdown(); });

  /* Components */
  Universe universe {
      {
          new Monitor { &universe },
          new Camera { &universe },
          new Generator { &universe },
          new Lamp { &universe },
          new Lamp { &universe },
          new CPU { &universe },
          new Terminal { &universe },
          new Splitter { &universe },
          new Switch { &universe },
      },
      {
          new VideoSystem { &universe },
          new EnergySystem { &universe },
          new TextSystem { &universe },
      },
      {
          new Wireless { &universe },
          new Manual { &universe },
          new Wiring { &universe },
      },
  };

  universe.infrastructure<Manual>().load("connections.json");

  /* Main loop */
  bool done = false;
  while (!done) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        done = true;
      }
      ImGui_ImplSdlGL3_ProcessEvent(&e);
    }

    ImGui_ImplSdlGL3_NewFrame(window);

    /* Rendering */
    glClearColor(0.17f, 0.24f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Game tick */
    universe.tick();
    universe.render();

    DrawGraph(universe);
    SystemUI(universe);

    ImGui::Render();
    SDL_GL_SwapWindow(window);
  }

  universe.infrastructure<Manual>().save("connections.json");

  return 0;
}
