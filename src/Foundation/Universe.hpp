#pragma once

#include <vector>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <fmt/format.h>

#include <Foundation/Infrastructure/Infrastructure.hpp>
#include <Foundation/Components/Component.hpp>
#include <Foundation/Systems/System.hpp>

struct Universe {
  Universe(const std::vector<Component*>& components, const std::vector<System*>& systems, const std::vector<Infrastructure*>& infrastructures);
  ~Universe();

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

  Port& lookupPort(const std::string& component, const std::string& port);

  void tick();

  void save(const fs::path& path);
  void load(const fs::path& path);

  void connect(Port& a, Port& b, Capabilities capabilities);
  void disconnect(Port& a, Port& b);
  bool connected(Port& a, Port& b) const;

  std::vector<Infrastructure*> infrastructures;
  std::vector<Component*> components;
  std::vector<System*> systems;
  std::vector<Connection> connections;
};
