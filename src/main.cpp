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
#include <Foundation/Systems/System.hpp>
#include <Foundation/Systems/Picture.hpp>
#include <Foundation/Systems/Energy.hpp>
#include <Foundation/Systems/Text.hpp>

struct World {
  // TODO: Destructor

  template <typename T>
  T& get() {
    for (System* s : systems) {
      if (auto t = dynamic_cast<T*>(s)) {
        return *t;
      }
    }
    throw std::runtime_error {
        fmt::format("World doesn't own system of type '{}'", typeid(T).name())
    };
  }

  std::vector<Component*> components;
  std::vector<System*> systems;
};

struct Monitor : public Component {
  glm::vec3 color;
  std::string message;

  explicit Monitor(World* w)
    : Component(w)
    , port(this, Capabilities { { true, 0.0f }, { false, 0.0f }, { true } })
  { }

  void update() override {
    if (auto data = world->get<Picture::System>().receive(&this->port)) {
      color = *data;
    }
    else {
      color = 0.5f * randomColor();
    }

    while (auto msg = this->world->get<Text::System>().receive(&this->port)) {
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
  explicit Camera(World* w)
    : Component(w)
    , port(this, { { true, 0.0f }, { false, 0.0f } })
  { }

  void update() override {
    world->get<Picture::System>().send(&this->port, color);
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
  explicit Generator(World* w)
    : Component(w)
    , port(this, { { false, 0.0f }, { true, 100.0f } })
  { }

  void update() override {
    this->world->get<Energy::System>().offer(&this->port, power);
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
  explicit Lamp(World* w)
    : Component(w)
    , port(this, { { false, 0.0f }, { true, 10.0f } })
  { }

  void update() override {
    float energy = this->world->get<Energy::System>().request(&this->port, 10.0f);
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
  explicit Terminal(World* w)
    : Component(w)
    , port(this, { { false, 0.0f }, { false, 0.0f }, { true } })
  { }

  void update() override { }

  void render() override {
    ImGui::Begin("Terminal");
    char buf[256] {};
    if (ImGui::InputText("Text", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
      this->world->get<Text::System>().send(&this->port, buf);
    }
    ImGui::End();
  }

  Wiring::Port port;
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
  World world {
      .components = {
          new Monitor { &world },
          new Camera { &world },
          new Generator { &world },
          new Lamp { &world },
          new CPU { &world },
          new Terminal { &world },
      },
      .systems = {
          new Picture::System { &world },
          new Energy::System { &world },
          new Text::System { &world },
      }
  };

  Wiring::connect(
      static_cast<Monitor*>(world.components[0])->port,
      static_cast<Camera*>(world.components[1])->port
  );

  /* Main loop */
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    /* Rendering */
    glClearColor(0.17f, 0.24f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Game tick */
    for (auto& component : world.components) {
      component->update();
    }

    for (auto& system : world.systems) {
      system->update();
    }

    for (auto& component : world.components) {
      component->render();
    }

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  return 0;
}
