#pragma once

#include <fmt/format.h>

#include <Foundation/Components/Component.hpp>
#include <Foundation/Systems/System.hpp>
#include <Foundation/Infrastructure/Wiring.hpp>

struct Universe {
  Universe(const std::vector<Component*>&& components, const std::vector<System*>& systems)
    : components { components }
    , systems { systems }
  {}

  ~Universe() {
    for (Component* component : components) {
      delete component;
    }
    for (System* system : systems) {
      delete system;
    }
  }

  template <typename T>
  T& get() {
    for (System* s : systems) {
      if (auto t = dynamic_cast<T*>(s)) {
        return *t;
      }
    }
    throw std::runtime_error {
        fmt::format("Universe doesn't own system of type '{}'", typeid(T).name())
    };
  }

  Wiring::Port* lookupPort(const std::string& component, const std::string& port);

  void tick();

  std::vector<Component*> components;
  std::vector<System*> systems;
  std::vector<std::tuple<Component*, Component*, Wiring::Connection>> connections;
};
