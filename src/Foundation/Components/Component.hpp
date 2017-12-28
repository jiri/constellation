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

  Endpoint* port(const std::string& id);
  std::string nameOf(const Endpoint* p);

  virtual std::vector<std::pair<float, Endpoint*>> redistributeEnergy(Endpoint *port) { return {}; };

  Universe* universe = nullptr;
  std::map<std::string, std::unique_ptr<Endpoint>> ports;

  glm::vec2 position;

protected:
  void addPort(const std::string& name, Endpoint* p);

  Debugger debugger;
};
