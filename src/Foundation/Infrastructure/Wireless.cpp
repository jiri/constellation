#include <Foundation/Infrastructure/Wireless.hpp>

#include <Foundation/Universe.hpp>

void Wireless::update() {
  for (Component* a : universe->components) {
    for (Component* b : universe->components) {
      if (a == b) {
        continue;
      }

      Port& aPort = a->port(a->defaultPort());
      Port& bPort = b->port(b->defaultPort());

      glm::vec2 delta = b->position - a->position;

      if (glm::length(delta) == 0) {
        continue;
      }

      if (glm::length(delta) <= 50.0f) {
        float errorRate = std::fabs(std::max(glm::length(delta) - 40.0f, 0.0f) * 0.01f);

        Capabilities caps {
            .picture = { true, errorRate },
            .energy = { false, 0.0f },
            .text = { false },
        };

        universe->connect(aPort, bPort, caps);
      }
      else {
        universe->disconnect(aPort, bPort);
      }
    }
  }
}
