#pragma once

#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <Foundation/Debugger.hpp>
#include <Foundation/Infrastructures/Infrastructure.hpp>

class Universe;

struct Component {
  explicit Component(Universe* world);
  virtual ~Component();

  virtual void update();
  virtual void render() = 0;

  virtual std::string name() const = 0;
  virtual std::string defaultPort() const {
    return "debug";
  }

  Port* port(const std::string& id);
  std::string nameOf(const Port* p);

  virtual std::vector<std::pair<float, Port*>> redistributeEnergy(Port *port) { return {}; };

  Universe* universe = nullptr;
  std::map<std::string, std::unique_ptr<Port>> ports;

  glm::vec2 position;

protected:
  void addPort(const std::string& name, Port* p);

  Debugger debugger;
};
