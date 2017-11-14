#include <Foundation/Component.hpp>

#include <algorithm>

Component::Component()
    : vertex(Wiring::graph().addVertex({ this }))
{ }

Component::~Component() {
  Wiring::graph().vertices.erase(Wiring::graph().vertices.begin() + vertex);
}
