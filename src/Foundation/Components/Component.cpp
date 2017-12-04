#include <Foundation/Components/Component.hpp>

#include <algorithm>
#include <fmt/format.h>

Component::Component(Universe* w)
  : vertex(Wiring::graph().addVertex({ this }))
  , universe { w }
{ }

Component::~Component() {
  Wiring::graph().vertices.erase(Wiring::graph().vertices.begin() + vertex);
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
