#include <Foundation/Component.hpp>

Component::Component()
    : vertex(boost::add_vertex(Wiring::graph()))
{ }

Component::~Component() {
  boost::remove_vertex(this->vertex, Wiring::graph());
}
