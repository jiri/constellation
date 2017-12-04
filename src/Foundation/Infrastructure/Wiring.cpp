#include <Foundation/Infrastructure/Wiring.hpp>

#include <Foundation/Components/Component.hpp>
#include <fmt/format.h>

#pragma mark Port

Wiring::Port::Port(Capabilities cap)
  : Node(cap)
{ }

Wiring::Port* Wiring::Port::findPort(Node*) {
  return this;
}

bool Wiring::Port::isFree() const {
  return neighbour == nullptr;
}

void Wiring::Port::addNeighbour(Node* node) {
  assert(isFree());
  neighbour = node;
}

void Wiring::Port::removeNeighbour(Node* node) {
  assert(neighbour == node);
  neighbour = nullptr;
}
bool Wiring::Port::isNeighbourOf(Node* node) const {
  return neighbour == node;
}

std::optional<Capabilities> Wiring::Port::fold(Node* prev) {
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

bool Wiring::Port::connected() {
  return neighbour && neighbour->findPort(this);
}

#pragma mark Cable

Wiring::Port* Wiring::Cable::findPort(Node* prev) {
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

bool Wiring::Cable::isFree() const {
  return neighbourCount < 2;
}

void Wiring::Cable::addNeighbour(Node* node) {
  assert(isFree());
  neighbours[neighbourCount++] = node;
}

void Wiring::Cable::removeNeighbour(Node* node) {
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

bool Wiring::Cable::isNeighbourOf(Node* node) const {
  return neighbours[0] == node || neighbours[1] == node;
}

std::optional<Capabilities> Wiring::Cable::fold(Node* prev) {
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

  Port* left = a->findPort(b);
  Port* right = b->findPort(a);

  if (left && right) {
    Capabilities result = *left->fold(nullptr);
    graph().edges.emplace_back(left->component, right->component, Edge { left, right, result });
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
    auto it = std::find_if(graph().edges.begin(), graph().edges.end(), [left, right](const auto& e) -> bool {
      return (std::get<0>(e) == left->component && std::get<1>(e) == right->component)
          || (std::get<0>(e) == right->component && std::get<1>(e) == left->component);
    });
    assert(it != graph().edges.end());
    graph().edges.erase(it);
  }

  a->removeNeighbour(b);
  b->removeNeighbour(a);
}

void Wiring::connect(Node& a, Node& b) {
  connect(&a, &b);
}

void Wiring::disconnect(Node& a, Node& b) {
  disconnect(&a, &b);
}
