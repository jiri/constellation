#pragma once

#include <Foundation/Wiring.hpp>

class Universe;

struct Component {
  Component(Universe* world);
  virtual ~Component();

  virtual void update() = 0;
  virtual void render() = 0;

  Wiring::Graph::vertexHandle vertex;
  Universe* universe = nullptr;
};
