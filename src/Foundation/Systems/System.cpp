#include <Foundation/Systems/System.hpp>

#include <Foundation/Universe.hpp>

void System::update() {
  for (auto& connection : universe->connections) {
    if (this->filter(connection)) {
      this->swap(connection);
    }
  }
}
