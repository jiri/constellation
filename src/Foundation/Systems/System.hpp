#pragma once

#include <Foundation/Wiring.hpp>

class World;

struct System {
  System(World* w)
    : world { w }
  { }

  virtual ~System() = default;

  virtual bool filter(const Wiring::Edge& edge) const = 0;
  virtual void swap(Wiring::Edge& edge) = 0;

  void update() {
    for (auto& t : Wiring::graph().edges) {
      auto& edge = std::get<2>(t);
      if (this->filter(edge)) {
        this->swap(edge);
      }
    }
  }

  World* world;
};
