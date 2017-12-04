#include <Foundation/Components/Component.hpp>

#include <algorithm>
#include <fmt/format.h>

Component::Component(Universe* w)
  : universe { w }
{ }

Component::~Component() {
  Wiring::graph().edges.erase(
          std::remove_if(Wiring::graph().edges.begin(), Wiring::graph().edges.end(),
                         [this](const std::tuple<Component*, Component*, Wiring::Edge>& e) {
                           return std::get<0>(e) == this || std::get<1>(e) == this;
                         }),
          Wiring::graph().edges.end()
  );

  Wiring::graph().vertices.erase(
          std::remove(Wiring::graph().vertices.begin(), Wiring::graph().vertices.end(), this),
          Wiring::graph().vertices.end()
  );
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
