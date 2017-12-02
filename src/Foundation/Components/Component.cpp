#include <Foundation/Components/Component.hpp>

#include <algorithm>

Component::Component(Universe* w)
  : vertex(Wiring::graph().addVertex({ this }))
  , universe { w }
{ }

Component::~Component() {
  Wiring::graph().vertices.erase(Wiring::graph().vertices.begin() + vertex);
}
