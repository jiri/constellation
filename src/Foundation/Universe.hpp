#pragma once

#include <vector>
#include <chrono>

#include <fmt/format.h>

#include <Foundation/Infrastructures/Infrastructure.hpp>
#include <Foundation/Components/Component.hpp>
#include <Foundation/Systems/System.hpp>

struct Universe {
  template <typename T>
  T& system() {
    for (auto& s : systems) {
      if (auto t = dynamic_cast<T*>(s.get())) {
        return *t;
      }
    }
    throw std::runtime_error {
        fmt::format("Universe doesn't own system of type '{}'", typeid(T).name())
    };
  }

  template <typename T>
  T& infrastructure() {
    for (auto& i : infrastructures) {
      if (auto t = dynamic_cast<T*>(i.get())) {
        return *t;
      }
    }
    throw std::runtime_error {
        fmt::format("Universe doesn't own infrastructure of type '{}'", typeid(T).name())
    };
  }

  template <typename T, typename... Args>
  T* add(Args&&... args) {
    auto* ptr = new T { this, std::forward<Args>(args)... };

    if constexpr (std::is_base_of_v<Infrastructure, T>) {
      this->infrastructures.emplace_back(ptr);
    }
    else if constexpr (std::is_base_of_v<System, T>) {
      this->systems.emplace_back(ptr);
    }
    else if constexpr (std::is_base_of_v<Component, T>) {
      this->components.emplace_back(ptr);
    }
    else {
      throw std::domain_error { "Universe doesn't support adding this type" };
    }

    return ptr;
  }

  void tick();
  void render();

  Endpoint* lookupPort(const std::string& component, const std::string& port);

  std::vector<std::unique_ptr<Infrastructure>> infrastructures;
  std::vector<std::unique_ptr<Component>> components;
  std::vector<std::unique_ptr<System>> systems;

  std::vector<Connection> connections;

private:
  std::chrono::system_clock::time_point oldTime = std::chrono::system_clock::time_point::min();
};
