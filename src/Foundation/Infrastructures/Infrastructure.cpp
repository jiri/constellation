#include <Foundation/Infrastructures/Infrastructure.hpp>

#include <Foundation/Universe.hpp>

std::string Port::name() const {
  return component->nameOf(this);
}

glm::vec2 Port::globalPosition() const {
  return component->position + this->position;
}

void Infrastructure::connect(Port& a, Port& b, Capabilities capabilities) {
  if (connected(a, b)) {
    return;
  }

  universe->connections.emplace_back(&a, &b, a.capabilities * b.capabilities * capabilities, this);
}

void Infrastructure::disconnect(Port& a, Port& b) {
  auto pred = [&](const Connection& c) {
    return (c.a == &a && c.b == &b) || (c.a == &b && c.b == &a);
  };

  if (!connected(a, b)) {
    return;
  }

  auto pos = std::remove_if(universe->connections.begin(), universe->connections.end(), pred);
  universe->connections.erase(pos, universe->connections.end());
}

bool Infrastructure::connected(Port& a, Port& b) const {
  for (const Connection& c : universe->connections) {
    if ((c.a == &a && c.b == &b) || (c.a == &b && c.b == &a)) {
      return true;
    }
  }
  return false;
}

Connection* Infrastructure::connection(Port& a, Port& b) {
  for (Connection& c : universe->connections) {
    if ((c.a == &a && c.b == &b) || (c.a == &b && c.b == &a)) {
      return &c;
    }
  }
  return nullptr;
}
