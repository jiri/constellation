#include <Foundation/Infrastructures/Infrastructure.hpp>

#include <Foundation/Universe.hpp>

std::string Port::name() const {
  return component->nameOf(this);
}

glm::vec2 Port::globalPosition() const {
  return component->position + this->position;
}

void Infrastructure::connect(Port* from, Port* to, Capabilities capabilities) {
  Capabilities newCapabilities = from->capabilities * capabilities * to->capabilities;

  if (Connection* c = connection(from, to)) {
    c->capabilities = newCapabilities;
  }
  else {
    universe->connections.emplace_back(from, to, newCapabilities, this);
  }
}

void Infrastructure::disconnect(Port* from, Port* to) {
  auto pred = [&](const Connection& c) {
    return c.from == from && c.to == to;
  };

  auto pos = std::remove_if(universe->connections.begin(), universe->connections.end(), pred);
  universe->connections.erase(pos, universe->connections.end());
}

Connection* Infrastructure::connection(Port* from, Port* to) {
  for (Connection& c : universe->connections) {
    if (c.from == from && c.to == to) {
      return &c;
    }
  }
  return nullptr;
}
