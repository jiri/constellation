#pragma once

#include <string>
#include <vector>

#include <Foundation/Infrastructure/Wiring.hpp>

class Universe;

struct Component {
  explicit Component(Universe* world);
  virtual ~Component();

  virtual void update() = 0;
  virtual void render() = 0;

  virtual std::string name() const = 0;
  virtual std::vector<std::pair<std::string, Wiring::Port*>> ports() = 0;
  virtual std::string defaultPort() const = 0;

  Wiring::Graph::vertexHandle vertex;
  Universe* universe = nullptr;
};
