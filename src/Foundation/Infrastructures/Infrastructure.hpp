#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include <Foundation/Infrastructures/Capabilities.hpp>

class Component;
class Universe;

class Port {
public:
  explicit Port(Capabilities c)
    : capabilities { c }
  { }

  virtual ~Port() = default;

  std::string name() const;

  glm::vec2 globalPosition() const;

  Component* component = nullptr;
  Capabilities capabilities;
  glm::vec2 position;
};

class Infrastructure;

struct Connection {
  Connection(Port* f, Port* t, Capabilities c, Infrastructure* author)
    : from { f }
    , to { t }
    , capabilities { c }
    , author { author }
  { }

  Port* from;
  Port* to;
  Capabilities capabilities;
  Infrastructure* author;
};

class Infrastructure {
public:
  explicit Infrastructure(Universe* u)
    : universe { u }
  { }

  virtual ~Infrastructure() = default;

  virtual void update() = 0;

  void connect(Port* from, Port* to, Capabilities capabilities);
  void disconnect(Port* from, Port* to);

protected:
  Universe* universe;

  Connection* connection(Port* from, Port* to);
};
