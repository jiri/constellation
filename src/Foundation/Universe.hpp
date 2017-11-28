#pragma once

#include <fmt/format.h>

#include <Foundation/Component.hpp>
#include <Foundation/Systems/System.hpp>

struct Universe {
  // TODO: Destructor

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
};
