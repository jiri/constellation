#include <Foundation/Component.hpp>

#include <algorithm>

Component::Component(World* w)
  : vertex(Wiring::graph().addVertex({ this }))
  , world { w }
{ }

Component::~Component() {
  Wiring::graph().vertices.erase(Wiring::graph().vertices.begin() + vertex);
}
