#include <Foundation/Universe.hpp>

#include <stdexcept>
#include <thread>

#include <Foundation/Infrastructures/Manual.hpp>

Universe::Universe(std::vector<Component*> components,
                   std::vector<System*> systems,
                   std::vector<Infrastructure*> infrastructures)
  : components { std::move(components) }
  , systems { std::move(systems) }
  , infrastructures { std::move(infrastructures) }
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
  /* Compute delta time */
  // TODO: Make this per-instance
  static std::chrono::time_point oldTime = std::chrono::system_clock::now();
  std::chrono::time_point newTime = std::chrono::system_clock::now();
  std::chrono::system_clock::duration delta = newTime - oldTime;
  oldTime = newTime;

  /* Update components */
  for (auto& component : this->components) {
    component->update();
  }

  /* Update systems */
  std::vector<std::thread> systemThreads;
  for (auto& system : this->systems) {
    systemThreads.emplace_back(&System::timePassed, system, delta);
  }
  for (auto& t : systemThreads) {
    t.join();
  }

  /* Update infrastructures */
  for (auto& infrastructure : this->infrastructures) {
    infrastructure->update();
  }
}

void Universe::render() {
  for (auto& component : this->components) {
    component->render();
  }
}

Port* Universe::lookupPort(const std::string& componentName, const std::string& portName) {
  for (Component* c : this->components) {
    if (c->name() == componentName) {
      for (auto& [ name, port ] : c->ports) {
        if ((portName.empty() && name == c->defaultPort()) || name == portName) {
          return port.get();
        }
      }
    }
  }

  throw std::runtime_error {
      fmt::format("Component '{}' doesn't have port '{}'", componentName, portName)
  };
}
