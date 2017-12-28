#include <Foundation/Infrastructures/Manual.hpp>

#include <regex>
#include <fstream>

#include <imgui.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <json.hpp>

#include <Foundation/Universe.hpp>

void Manual::update() {
  ImGui::SetNextWindowPos({ 0.0f, ImGui::GetIO().DisplaySize.y - 36.0f });
  ImGui::SetNextWindowSize({ ImGui::GetIO().DisplaySize.x, 32.0f });
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4 { 0.0f, 0.0f, 0.0f, 0.0f });
  ImGui::Begin("Console", nullptr, ImGuiWindowFlags_NoTitleBar
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
      Endpoint* a = universe->lookupPort(m[2], m[4]);
      Endpoint* b = universe->lookupPort(m[5], m[7]);

      if (m[1] == "c") {
        connect(a, b, Capabilities{});
        connect(b, a, Capabilities{});
      }
      else if (m[1] == "d") {
        disconnect(a, b);
        disconnect(b, a);
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
  ImGui::PopStyleColor();
}

void Manual::save(const fs::path& path) {
  json cs;

  for (auto& connection : this->universe->connections) {
    if (connection.author != this) {
      continue;
    }

    json c = json {
        {
            "a", {
                     { "component", connection.from->component->name() },
                     { "port", connection.from->name() },
                 },
        },
        {
            "b", {
                     { "component", connection.to->component->name() },
                     { "port", connection.to->name() },
                 },
        },
    };

    cs.push_back(c);
  }

  std::ofstream outf { path };
  outf << cs << std::endl;
}

void Manual::load(const fs::path& path) {
  if (!fs::exists(path)) {
    return;
  }

  std::ifstream inf { path };

  json connections;
  inf >> connections;

  for (auto& connection : connections) {
    Endpoint* a = this->universe->lookupPort(connection["a"]["component"], connection["a"]["port"]);
    Endpoint* b = this->universe->lookupPort(connection["b"]["component"], connection["b"]["port"]);

    this->connect(a, b, Capabilities{});
    this->connect(b, a, Capabilities{});
  }
}
