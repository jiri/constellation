#include <Foundation/Components/Component.hpp>

#include <algorithm>
#include <fmt/format.h>

#include <Foundation/Universe.hpp>

Component::Component(Universe* u)
  : universe { u }
{ }

Component::~Component() {
  auto pred = [this](const Wiring::Connection& c) {
    return c.a->component == this || c.b->component == this;
  };

  auto beg = universe->connections.begin();
  auto end = universe->connections.end();
  universe->connections.erase(std::remove_if(beg, end, pred), end);
}

void Component::updatePorts() {
  for (auto& pair : ports) {
    pair.second.component = this;
  }
}

Wiring::Port& Component::port(const std::string& id) {
  if (ports.count(id) == 0) {
    throw std::runtime_error {
        fmt::format("Invalid port '{}:{}'", name(), id)
    };
  }
  return ports.at(id);
}

std::string Component::nameOf(const Wiring::Port* p) {
  for (auto& pair : ports) {
    if (&pair.second == p) {
      return pair.first;
    }
  }
  throw std::runtime_error { "Invalid port" };
}
