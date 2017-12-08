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

          glm::vec2 delta = b->position() - a->position();

          if (glm::length(delta) <= std::max(a->radius, b->radius)) {
            Capabilities caps {
                    .picture = { true, 0.0 },
                    .energy = { false, 0.0f },
                    .text = { false },
            };

            connect(*a, *b, Capabilities{});
          }
          else {
            disconnect(*a, *b);
          }
        }
      }
    }
  }
}
