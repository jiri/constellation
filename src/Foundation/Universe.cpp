#include <Foundation/Universe.hpp>

#include <fstream>
#include <stdexcept>

#include <json.hpp>

Universe::Universe(const std::vector<Component*>&& components, const std::vector<System*>& systems)
  : components { components }
  , systems { systems }
  , infrastructure { this }
{}

Universe::~Universe() {
  for (Component* component : components) {
    delete component;
  }
  for (System* system : systems) {
    delete system;
  }
}

void Universe::tick() {
  for (auto& component : this->components) {
    component->update();
  }

  for (auto& system : this->systems) {
    system->update();
  }

  for (auto& component : this->components) {
    component->render();
  }
}

Port& Universe::lookupPort(const std::string& componentName, const std::string& portName) {
  for (Component* c : this->components) {
    if (c->name() == componentName) {
      for (auto& [ name, port ] : c->ports) {
        if ((portName.empty() && name == c->defaultPort()) || name == portName) {
          return port;
        }
      }
    }
  }

  throw std::runtime_error {
      fmt::format("Component '{}' doesn't have port '{}'")
  };
}

void Universe::save(const fs::path& path) {
  json cs;

  for (auto& connection : connections) {
    json c = json {
        {
            "a", {
                { "component", connection.a->component->name() },
                { "port", connection.a->name() },
            },
        },
        {
            "b", {
                { "component", connection.b->component->name() },
                { "port", connection.b->name() },
            },
        },
    };

    cs.push_back(c);
  }

  std::ofstream outf { path };
  outf << cs << std::endl;
}

void Universe::load(const fs::path& path) {
  if (!fs::exists(path)) {
    return;
  }

  std::ifstream inf { path };

  json connections;
  inf >> connections;

  for (auto& connection : connections) {
    Port& a = lookupPort(connection["a"]["component"], connection["a"]["port"]);
    Port& b = lookupPort(connection["b"]["component"], connection["b"]["port"]);

    connect(a, b, Capabilities{});
  }
}

void Universe::connect(Port& a, Port& b, Capabilities capabilities) {
  if (connected(a, b)) {
    return;
  }

  connections.emplace_back(&a, &b, a.capabilities * b.capabilities * capabilities);
}

void Universe::disconnect(Port& a, Port& b) {
  static auto pred = [&a, &b](const Connection& c) {
    return (c.a == &a && c.b == &b) || (c.a == &b && c.b == &a);
  };

  if (!connected(a, b)) {
    return;
  }

  auto pos = std::find_if(connections.begin(), connections.end(), pred);
  connections.erase(pos, connections.end());
}

bool Universe::connected(Port& a, Port& b) const {
  static auto pred = [&a, &b](const Connection& c) {
    return (c.a == &a && c.b == &b) || (c.a == &b && c.b == &a);
  };
  auto pos = std::find_if(connections.begin(), connections.end(), pred);
  return pos != connections.end();
}
