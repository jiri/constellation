#include <Foundation/Universe.hpp>

#include <fstream>
#include <stdexcept>

#include <json.hpp>

Universe::Universe(const std::vector<Component*>& components,
                   const std::vector<System*>& systems,
                   const std::vector<Infrastructure*>& infrastructures)
  : components { components }
  , systems { systems }
  , infrastructures { infrastructures }
{}

Universe::~Universe() {
  for (Infrastructure* infrastructure : infrastructures) {
    delete infrastructure;
  }
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

  for (auto& infrastructure : this->infrastructures) {
    infrastructure->update();
  }
}

Port& Universe::lookupPort(const std::string& componentName, const std::string& portName) {
  for (Component* c : this->components) {
    if (c->name() == componentName) {
      for (auto& [ name, port ] : c->ports) {
        if ((portName.empty() && name == c->defaultPort()) || name == portName) {
          return *port;
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

//    connect(a, b, Capabilities{});
  }
}
