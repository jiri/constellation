#include <Foundation/Infrastructures/Wiring.hpp>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <Foundation/Universe.hpp>

Cable::Cable(Wiring* w, glm::vec2 aPos, glm::vec2 bPos, Capabilities caps)
  : wiring { w }
  , capabilities { caps }
  , a { this, aPos }
  , b { this, bPos }
{
  wiring->join(&a, &b);
}

Cable::~Cable() {
  wiring->separate(&a, &b);
}

Wiring::~Wiring() {
  for (auto* cable : cables) {
    delete cable;
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
  /* Update connections */
  for (auto& c : universe->components) {
    for (auto& pair : c->ports) {
      if (auto* s = dynamic_cast<Socket*>(pair.second.get())) {
        Socket* other = findOther(s);
        if (other) {
          connect(s, other, Capabilities{});
        }
      }
    }
  }

  for (auto& c : universe->connections) {
    auto* as = dynamic_cast<Socket*>(c.from);
    auto* bs = dynamic_cast<Socket*>(c.to);

    if (as && bs && (findOther(as) != bs || findOther(bs) != as)) {
      disconnect(as, bs);
    }
  }

  /* Connect close cables */
  for (auto* a : cables) {
    for (auto* b : cables) {
      if (a == b) {
        continue;
      }

      if (glm::distance(a->a.position, b->a.position) < 4.0f) {
        join(&a->a, &b->a);
      }
      else if (glm::distance(a->a.position, b->a.position) > 30.0f) {
        separate(&a->a, &b->a);
      }

      if (glm::distance(a->a.position, b->b.position) < 4.0f) {
        join(&a->a, &b->b);
      }
      else if (glm::distance(a->a.position, b->b.position) > 30.0f) {
        separate(&a->a, &b->b);
      }

      if (glm::distance(a->b.position, b->a.position) < 4.0f) {
        join(&a->b, &b->a);
      }
      else if (glm::distance(a->b.position, b->a.position) > 30.0f) {
        separate(&a->b, &b->a);
      }

      if (glm::distance(a->b.position, b->b.position) < 4.0f) {
        join(&a->b, &b->b);
      }
      else if (glm::distance(a->b.position, b->b.position) > 30.0f) {
        separate(&a->b, &b->b);
      }
    }
  }

  static std::unordered_map<Connector*, glm::vec2> oldPositions;

  /* Keep connectors together */
  for (auto& c : connections) {
    auto* a = dynamic_cast<Connector*>(c.first);
    auto* b = dynamic_cast<Connector*>(c.second);
    auto* as = dynamic_cast<Socket*>(c.first);
    auto* bs = dynamic_cast<Socket*>(c.second);

    if (a && b && a->cable != b->cable) {
      if (oldPositions[a] != a->position) {
        b->position = a->position;
        oldPositions[a] = a->position;
        oldPositions[b] = b->position;
      }
      if (oldPositions[b] != b->position) {
        a->position = b->position;
        oldPositions[a] = a->position;
        oldPositions[b] = b->position;
      }
    }

    if (a && bs) {
      if (glm::distance(a->position, bs->globalPosition()) > 10.0f) {
        separate(a, bs);
      }
      else {
        a->position = bs->globalPosition();
      }
    }

    if (as && b) {
      if (glm::distance(a->position, bs->globalPosition()) > 10.0f) {
        separate(as, b);
      }
      else {
        b->position = as->globalPosition();
      }
    }
  }

  ImGui::Begin("Switchboard");

  if (ImGui::Button("Add Cable")) {
    cables.emplace_back(new Cable { this, glm::vec2 { 0.0f, 0.0f }, glm::vec2 { 50.0f, 0.0f }, Capabilities{} });
  }

  ImGui::End();
}

void Wiring::join(Node* a, Node* b) {
  auto pred = [&](const std::pair<Node*, Node*>& p) {
    return (p.first == a && p.second == b) || (p.first == b && p.second == a);
  };

  auto it = std::find_if(connections.begin(), connections.end(), pred);
  if (it == connections.end()) {
    connections.emplace_back(a, b);
  }
}

void Wiring::separate(Node* a, Node* b) {
  auto pred = [&](const std::pair<Node*, Node*>& p) {
    return (p.first == a && p.second == b) || (p.first == b && p.second == a);
  };

  auto it = std::remove_if(connections.begin(), connections.end(), pred);
  connections.erase(it, connections.end());
}
