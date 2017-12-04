#pragma once

#include <map>
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
  virtual std::string defaultPort() const = 0;

  Wiring::Port& port(const std::string& id);

  Universe* universe = nullptr;
  std::map<std::string, Wiring::Port> ports;

protected:
  void updatePorts();
};
