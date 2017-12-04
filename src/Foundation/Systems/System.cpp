#include <Foundation/Systems/System.hpp>

#include <Foundation/Universe.hpp>

void System::update() {
  for (auto& t : universe->graph.edges) {
    auto& edge = std::get<2>(t);
    if (this->filter(edge)) {
      this->swap(edge);
    }
  }
}
