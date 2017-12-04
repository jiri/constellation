#include <Foundation/Universe.hpp>

void Universe::tick() {
  for (auto& component : this->components) {
    component->update();
  }

  for (auto& system : this->systems) {
    system->update();
  }

  for (auto& component : this->components) {
    component->render();
  }
}

Wiring::Port* Universe::lookupPort(const std::string& componentName, const std::string& portName) {
  for (Component* c : this->components) {
    if (c->name() == componentName) {
      for (auto& [ name, port ] : c->ports) {
        if ((portName.empty() && name == c->defaultPort()) || name == portName) {
          return &port;
        }
      }
    }
  }

  return nullptr;
}
