#include <Foundation/Infrastructures/Wiring.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <Foundation/Universe.hpp>

Cable::Cable(Wiring* w, Capabilities caps)
        : wiring { w }
        , capabilities { caps }
{
  wiring->connections.emplace_back(&a, &b);
}

Cable::~Cable() {
  for (auto it = wiring->connections.begin(); it != wiring->connections.end(); it++) {
    if (it->first == &a && it->second == &b) {
      wiring->connections.erase(it);
    }
  }
}

Socket* Wiring::findOther(Socket* socket) {
  Node* prev = socket;
  Node* next = nullptr;

  for (auto& connection : connections) {
    if (connection.first == socket) {
      next = connection.second;
    }
    if (connection.second == socket) {
      next = connection.first;
    }
  }

  bool stepMade = false;
  do {
    stepMade = false;
    for (auto& connection : connections) {
      if (connection.first == next || connection.second == next) {
        if (connection.first == prev || connection.second == prev) {
          continue;
        }
        else if (connection.first == next) {
          prev = next;
          next = connection.second;
          stepMade = true;
        }
        else if (connection.second == next) {
          prev = next;
          next = connection.first;
          stepMade = true;
        }
      }
    }
  } while (stepMade);

  return dynamic_cast<Socket*>(next);
}

void Wiring::update() {
  for (auto& c : universe->components) {
    for (auto& pair : c->ports) {
      if (auto* s = dynamic_cast<Socket*>(pair.second.get())) {
        Socket* other = findOther(s);
        if (other) {
          connect(*s, *other, Capabilities{});
        }
      }
    }
  }

  ImGui::Begin("Switchboard");

  if (ImGui::Button("Connect")) {
    cables.emplace_back(this, Capabilities{});

    auto* a = dynamic_cast<Socket*>(&universe->lookupPort("generator", "energy"));
    auto* b = dynamic_cast<Socket*>(&universe->lookupPort("lamp", "energy"));

    assert(a && b);

    connections.emplace_back(a, &cables[0].a);
    connections.emplace_back(&cables[0].b, b);
  }

  ImGui::End();
}
