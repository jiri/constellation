#include <Foundation/Components/Component.hpp>

#include <algorithm>
#include <fmt/format.h>

#include <Foundation/Universe.hpp>

#include <Foundation/Infrastructures/Wiring.hpp>

Component::Component(Universe* u)
  : universe { u }
  , debugger { this, "debug" }
{
  addPort("debug", new Socket(Capabilities {
      .picture = { false, 0.0f },
      .energy = { false, 0.0f },
      .text = { true },
  }));
}

Component::~Component() {
  auto pred = [this](const Connection& c) {
    return c.from->component == this || c.to->component == this;
  };

  auto beg = universe->connections.begin();
  auto end = universe->connections.end();
  universe->connections.erase(std::remove_if(beg, end, pred), end);
}

void Component::addPort(const std::string& name, Endpoint* p) {
  p->component = this;
  ports.emplace(name, p);
}

Endpoint* Component::port(const std::string& id) {
  if (ports.count(id) == 0) {
    throw std::runtime_error {
        fmt::format("Invalid port '{}:{}'", name(), id)
    };
  }
  return ports.at(id).get();
}

std::string Component::nameOf(const Endpoint* p) {
  for (auto& pair : ports) {
    if (pair.second.get() == p) {
      return pair.first;
    }
  }
  throw std::runtime_error { "Invalid port" };
}

void Component::update() {
  debugger.update();
}
