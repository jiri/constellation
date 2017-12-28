#pragma once

#include <cstdint>
#include <vector>

#include <glm/glm.hpp>

#include <Foundation/Infrastructures/Capabilities.hpp>

class Component;
class Universe;

class Endpoint {
public:
  explicit Endpoint(Capabilities c)
    : capabilities { c }
  { }

  virtual ~Endpoint() = default;

  std::string name() const;

  glm::vec2 globalPosition() const;

  Component* component = nullptr;
  Capabilities capabilities;
  glm::vec2 position;
};

class Infrastructure;

struct Connection {
  Connection(Endpoint* f, Endpoint* t, Capabilities c, Infrastructure* author)
    : from { f }
    , to { t }
    , capabilities { c }
    , author { author }
  { }

  Endpoint* from;
  Endpoint* to;
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

  void connect(Endpoint* from, Endpoint* to, Capabilities capabilities);
  void disconnect(Endpoint* from, Endpoint* to);

protected:
  Universe* universe;

  Connection* connection(Endpoint* from, Endpoint* to);
};
