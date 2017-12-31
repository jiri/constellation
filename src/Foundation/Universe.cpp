#include <Foundation/Universe.hpp>

#include <stdexcept>
#include <thread>

#include <Foundation/Infrastructures/Manual.hpp>

void Universe::tick() {
  /* Compute delta time */
  if (this->oldTime == std::chrono::system_clock::time_point::min()) {
    this->oldTime = std::chrono::system_clock::now();
  }
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
    systemThreads.emplace_back(&System::timePassed, system.get(), delta);
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

Endpoint* Universe::lookupPort(const std::string& componentName, const std::string& portName) {
  for (auto& c : this->components) {
    if (c->name() == componentName) {
      for (auto& pair : c->ports) {
        auto& name = pair.first;
        auto& port = pair.second;

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
