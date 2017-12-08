#include <Foundation/Infrastructures/Debug.hpp>

#include <regex>

#include <imgui.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <Foundation/Universe.hpp>

void Debug::update() {
  ImGui::SetNextWindowPos({ 0.0f, 600 - 36.0f });
  ImGui::Begin("Console", nullptr, { 800.0f, 32.0f }, 0.0f,
               ImGuiWindowFlags_NoTitleBar
               | ImGuiWindowFlags_NoResize
               | ImGuiWindowFlags_NoScrollbar
               | ImGuiWindowFlags_NoSavedSettings
               | ImGuiWindowFlags_NoFocusOnAppearing);

  ImGui::PushItemWidth(-1);
  if (ImGui::InputText("##command", buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
    std::string buffer(buf);
    std::regex r(R"(^\s*([cd])\s+(\w+)(:(\w*))?\s+(\w+)(:(\w*))?)");
    std::smatch m;
    std::regex_search(buffer, m, r);

    try {
      Port& a = universe->lookupPort(m[2], m[4]);
      Port& b = universe->lookupPort(m[5], m[7]);

      if (m[1] == "c") {
        connect(a, b, Capabilities{});
      }
      else if (m[1] == "d") {
        disconnect(a, b);
      }
      else {
        fmt::print("Unknown command '{}'\n", m[1]);
      }
    }
    catch (std::runtime_error& e) {
      fmt::print("{}\n", e.what());
    }

    buf[0] = 0;
    ImGui::SetKeyboardFocusHere(-1);
  }

  ImGui::PopItemWidth();
  ImGui::End();
}
