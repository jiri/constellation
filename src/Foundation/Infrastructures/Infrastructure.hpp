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
  Connection(Port* a, Port* b, Capabilities c, Infrastructure* author)
    : a { a }
    , b { b }
    , capabilities { c }
    , author { author }
  { }

  Port* a;
  Port* b;
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

  void connect(Port* a, Port* b, Capabilities capabilities);
  void disconnect(Port* a, Port* b);

protected:
  Universe* universe;

  Connection* connection(Port* a, Port* b);
};
