#include <Foundation/Universe.hpp>

#include <fstream>

#include <json.hpp>
using json = nlohmann::json;

Universe::Universe(const std::vector<Component*>&& components, const std::vector<System*>& systems)
  : components { components }
  , systems { systems }
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

Wiring::Port* Universe::lookupPort(const std::string& componentName, const std::string& portName) {
  for (Component* c : this->components) {
    if (c->name() == componentName) {
      for (auto& [ name, port ] : c->ports) {
        if ((portName.empty() && name == c->defaultPort()) || name == portName) {
          return &port;
        }
      }
    }
  }

  return nullptr;
}

void Universe::save(const fs::path& path) {
  json cs;

  for (auto& connection : connections) {
    Component* a = std::get<0>(connection);
    Wiring::Port* aPort = std::get<2>(connection).a;
    Component* b = std::get<1>(connection);
    Wiring::Port* bPort = std::get<2>(connection).b;

    json c = json {
        {
            "a", {
                { "component", a->name() },
                { "port", a->nameOf(aPort) },
            },
        },
        {
            "b", {
                { "component", b->name() },
                { "port", b->nameOf(bPort) },
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
    Wiring::Port* a = lookupPort(connection["a"]["component"], connection["a"]["port"]);
    Wiring::Port* b = lookupPort(connection["b"]["component"], connection["b"]["port"]);

    Wiring::connect(*this, a, b);
  }
}
