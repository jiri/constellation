#pragma once

#include <vector>
#include <chrono>

#include <fmt/format.h>

#include <Foundation/Infrastructures/Infrastructure.hpp>
#include <Foundation/Components/Component.hpp>
#include <Foundation/Systems/System.hpp>

struct Universe {
  Universe(std::vector<Component*> components,
           std::vector<System*> systems,
           std::vector<Infrastructure*> infrastructures);
  ~Universe();

  template <typename T>
  T& system() {
    for (System* s : systems) {
      if (auto t = dynamic_cast<T*>(s)) {
        return *t;
      }
    }
    throw std::runtime_error {
        fmt::format("Universe doesn't own system of type '{}'", typeid(T).name())
    };
  }

  template <typename T>
  T& infrastructure() {
    for (Infrastructure* i : infrastructures) {
      if (auto t = dynamic_cast<T*>(i)) {
        return *t;
      }
    }
    throw std::runtime_error {
        fmt::format("Universe doesn't own infrastructure of type '{}'", typeid(T).name())
    };
  }

  void tick();
  void render();

  Endpoint* lookupPort(const std::string& component, const std::string& port);

  std::vector<Infrastructure*> infrastructures;
  std::vector<Component*> components;
  std::vector<System*> systems;
  std::vector<Connection> connections;

private:
  std::chrono::system_clock::time_point oldTime = std::chrono::system_clock::time_point::min();
};
