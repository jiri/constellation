#include <Foundation/Infrastructure/Infrastructure.hpp>

#include <Foundation/Universe.hpp>

std::string Port::name() const {
  return component->nameOf(this);
}

void Infrastructure::connect(Port& a, Port& b, Capabilities capabilities) {
  if (connected(a, b)) {
    return;
  }

  universe->connections.emplace_back(&a, &b, a.capabilities * b.capabilities * capabilities);
}

void Infrastructure::disconnect(Port& a, Port& b) {
  static auto pred = [&a, &b](const Connection& c) {
    return (c.a == &a && c.b == &b) || (c.a == &b && c.b == &a);
  };

  if (!connected(a, b)) {
    return;
  }

  auto pos = std::find_if(universe->connections.begin(), universe->connections.end(), pred);
  universe->connections.erase(pos, universe->connections.end());
}

bool Infrastructure::connected(Port& a, Port& b) const {
  static auto pred = [&a, &b](const Connection& c) {
    return (c.a == &a && c.b == &b) || (c.a == &b && c.b == &a);
  };
  auto pos = std::find_if(universe->connections.begin(), universe->connections.end(), pred);
  return pos != universe->connections.end();
}
