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
#include <Foundation/Universe.hpp>
#include <Foundation/Components/CPU.hpp>
#include <Foundation/Systems/Picture.hpp>
#include <Foundation/Systems/Energy.hpp>
#include <Foundation/Systems/Text.hpp>

struct Monitor : public Component {
  explicit Monitor(Universe* w)
    : Component(w)
    , port {
          this,
          Capabilities {
              .picture = { true, 0.0f },
              .energy = { false, 0.0f },
              .text = { true },
          }
      }
  { }

  void update() override {
    if (auto data = universe->get<Picture::System>().receive(&this->port)) {
      color = *data;
    }
    else {
      color = 0.5f * randomColor();
    }

    while (auto msg = this->universe->get<Text::System>().receive(&this->port)) {
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

  glm::vec3 color;
  std::string message;

  Wiring::Port port;
};

struct Camera : public Component {
  explicit Camera(Universe* w)
    : Component(w)
    , port {
          this,
          Capabilities {
              .picture = { true, 0.0f },
              .energy = { false, 0.0f },
              .text = { true },
          },
      }
  { }

  void update() override {
    universe->get<Picture::System>().send(&this->port, color);
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
  explicit Generator(Universe* w)
    : Component(w)
    , port {
          this,
          Capabilities {
              .picture = { false, 0.0f },
              .energy = { true, 100.0f },
              .text = { true }
          },
      }
  { }

  void update() override {
    this->universe->get<Energy::System>().offer(&this->port, power);
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
  explicit Lamp(Universe* w)
    : Component(w)
    , port {
          this,
          Capabilities {
              .picture = { false, 0.0f },
              .energy = { true, 10.0f },
              .text = { true },
          },
      }
  { }

  void update() override {
    float energy = this->universe->get<Energy::System>().request(&this->port, 10.0f);
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
  explicit Terminal(Universe* w)
    : Component(w)
    , port {
          this,
          Capabilities {
              .picture = { false, 0.0f },
              .energy = { false, 0.0f },
              .text = { true },
          },
      }
  { }

  void update() override { }

  void render() override {
    ImGui::Begin("Terminal");
    char buf[256] {};
    if (ImGui::InputText("Text", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
      this->universe->get<Text::System>().send(&this->port, buf);
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
  Universe universe {
      .components = {
          new Monitor { &universe },
          new Camera { &universe },
          new Generator { &universe },
          new Lamp { &universe },
          new CPU { &universe },
          new Terminal { &universe },
      },
      .systems = {
          new Picture::System { &universe },
          new Energy::System { &universe },
          new Text::System { &universe },
      }
  };

  /* Main loop */
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    /* Rendering */
    glClearColor(0.17f, 0.24f, 0.31f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Game tick */
    universe.tick();

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  return 0;
}
