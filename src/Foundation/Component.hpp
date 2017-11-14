#pragma once

#include <Foundation/Wiring.hpp>

struct Component {
  Component();
  virtual ~Component();

  virtual void update() = 0;
  virtual void render() const = 0;

  Wiring::Graph::vertex_descriptor vertex;
};
