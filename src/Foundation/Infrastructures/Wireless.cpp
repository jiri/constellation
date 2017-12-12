#include <Foundation/Infrastructures/Wireless.hpp>

#include <Foundation/Universe.hpp>

void Wireless::update() {
  for (Component* componentA : universe->components) {
    for (Component* componentB : universe->components) {
      if (componentA >= componentB) {
        continue;
      }

      for (auto& aPair : componentA->ports) {
        for (auto& bPair : componentB->ports) {
          auto* a = dynamic_cast<Antenna*>(aPair.second.get());
          auto* b = dynamic_cast<Antenna*>(bPair.second.get());

          if (!a || !b) {
            continue;
          }

          float distance = glm::length(b->globalPosition() - a->globalPosition());
          if (distance <= std::max(a->radius, b->radius)) {
            Capabilities caps {
                .picture = { true, std::fabs(a->frequency - b->frequency) },
                .energy = { false, 0.0f },
                .text = { false },
            };

            connect(a, b, caps);
          }
          else if (auto conn = connection(a, b)) {
            if (conn->author == this) {
              disconnect(a, b);
            }
          }
        }
      }
    }
  }
}
