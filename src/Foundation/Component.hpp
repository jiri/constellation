#pragma once

#include <Foundation/Wiring.hpp>

class World;

struct Component {
  Component(World* world);
  virtual ~Component();

  virtual void update() = 0;
  virtual void render() = 0;

  Wiring::Graph::vertexHandle vertex;
  World* world = nullptr;
};
