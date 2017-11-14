#include <Foundation/Wiring.hpp>

#include <Foundation/Component.hpp>
#include <fmt/format.h>

using namespace Wiring;

#pragma mark Port

Port::Port(Component* c, Capabilities cap)
  : Wiring::Node(cap)
  , component(c)
{ }

Port* Port::findPort(Node*) {
  return this;
}

bool Port::isFree() const {
  return neighbour == nullptr;
}

void Port::addNeighbour(Node* node) {
  assert(isFree());
  neighbour = node;
}

void Port::removeNeighbour(Node* node) {
  assert(neighbour == node);
  neighbour = nullptr;
}
bool Port::isNeighbourOf(Node* node) const {
  return neighbour == node;
}

std::optional<Capabilities> Port::fold(Node* prev) {
  if (prev == nullptr) {
    if (neighbour == nullptr) {
      return std::nullopt;
    }
    else if (auto other = neighbour->fold(this)) {
      return Capabilities::combine(capabilities, *other);
    }
    else {
      return std::nullopt;
    }
  }
  else {
    return { capabilities };
  }
}

bool Port::connected() {
  return neighbour && neighbour->findPort(this);
}

#pragma mark Cable

Port* Cable::findPort(Node* prev) {
  assert(prev != nullptr &&
         (neighbours[0] == prev || neighbours[1] == prev));

  if (neighbours[0] == nullptr || neighbours[1] == nullptr) {
    return nullptr;
  }

  if (prev == neighbours[0]) {
    return neighbours[1]->findPort(this);
  }
  else {
    return neighbours[0]->findPort(this);
  }
}

bool Cable::isFree() const {
  return neighbourCount < 2;
}

void Cable::addNeighbour(Node* node) {
  assert(isFree());
  neighbours[neighbourCount++] = node;
}

void Cable::removeNeighbour(Node* node) {
  assert(neighbours[0] == node || neighbours[1] == node);

  if (neighbours[1] == node) {
    neighbours[1] = nullptr;
    neighbourCount--;
  }
  else {
    neighbours[0] = neighbours[1];
    neighbours[1] = nullptr;
    neighbourCount--;
  }
}

bool Cable::isNeighbourOf(Node* node) const {
  return neighbours[0] == node || neighbours[1] == node;
}

std::optional<Capabilities> Cable::fold(Node* prev) {
  assert(prev == nullptr || prev == neighbours[0] || prev == neighbours[1]);

  if (neighbourCount < 2) {
    return std::nullopt;
  }

  auto ra = neighbours[0]->fold(this);
  auto rb = neighbours[1]->fold(this);

  if (prev == nullptr) {
    if (ra && rb) {
      return Capabilities::combine(capabilities, Capabilities::combine(*ra, *rb));
    }
    else {
      return std::nullopt;
    }
  }
  else {
    if (prev == neighbours[0]) {
      return Capabilities::combine(capabilities, *rb);
    }
    else {
      return Capabilities::combine(capabilities, *ra);
    }
  }
}

#pragma mark Utility

void Wiring::connect(Node* a, Node* b) {
  if (a->isNeighbourOf(b) && b->isNeighbourOf(a)) {
    return;
  }

  assert(a != b);
  assert(a->isFree() && b->isFree());

  a->addNeighbour(b);
  b->addNeighbour(a);

  Wiring::Port* left = a->findPort(b);
  Wiring::Port* right = b->findPort(a);

  if (left && right) {
    Capabilities result = *left->fold(nullptr);
    boost::add_edge(left->component->vertex, right->component->vertex, boost::property<edge_property_t, Edge> {
        Edge { left, right, result }
    }, Wiring::graph());
  }
}

void Wiring::disconnect(Node* a, Node* b) {
  assert(a != b);
  assert(a->isNeighbourOf(b) == b->isNeighbourOf(a));

  if (!a->isNeighbourOf(b) || !b->isNeighbourOf(a)) {
    return;
  }

  Port* left = a->findPort(b);
  Port* right = b->findPort(a);

  if (left && right) {
    auto edge = boost::edge(left->component->vertex, right->component->vertex, Wiring::graph());
    assert(edge.second);
    boost::remove_edge(edge.first, Wiring::graph());
  }

  a->removeNeighbour(b);
  b->removeNeighbour(a);
}

void Wiring::connect(Node& a, Node& b) {
  connect(&a, &b);
}

void Wiring::disconnect(Node& a, Node& b) {
  Wiring::disconnect(&a, &b);
}
