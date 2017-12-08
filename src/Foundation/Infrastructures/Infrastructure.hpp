#pragma once

#include <cstdint>
#include <vector>

#include <Foundation/Infrastructures/Capabilities.hpp>

class Component;
class Universe;

class Port {
public:
  Port(Capabilities c)
    : capabilities { c }
  { }

  std::string name() const;

  Component* component = nullptr;
  Capabilities capabilities;
};

struct Connection {
  Connection(Port* a, Port* b, Capabilities c)
    : a { a }
    , b { b }
    , capabilities { c }
  { }

  Port* a = nullptr;
  Port* b = nullptr;
  Capabilities capabilities;
};

class Infrastructure {
public:
  explicit Infrastructure(Universe* u)
    : universe { u }
  { }

  virtual ~Infrastructure() = default;

  virtual void update() = 0;

  void connect(Port& a, Port& b, Capabilities capabilities);
  void disconnect(Port& a, Port& b);
  bool connected(Port& a, Port& b) const;

protected:
  Universe* universe;
};
