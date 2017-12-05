#pragma once

#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <Foundation/Infrastructure/Infrastructure.hpp>

class Universe;

struct Component {
  explicit Component(Universe* world);
  virtual ~Component();

  virtual void update() = 0;
  virtual void render() = 0;

  virtual std::string name() const = 0;
  virtual std::string defaultPort() const = 0;

  Port& port(const std::string& id);
  std::string nameOf(const Port* p);

  Universe* universe = nullptr;
  std::map<std::string, Port> ports;

  glm::vec2 position;

protected:
  void updatePorts();
};
