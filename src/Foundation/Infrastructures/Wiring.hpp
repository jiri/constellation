#pragma once

#include <list>

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Wiring;

class Node {
public:
  virtual ~Node() = default;
};

class Socket : public Port, public Node {
public:
  using Port::Port;
};

class Connector : public Node {
public:
  glm::vec2 position;
};

class Cable {
public:
  Cable(Wiring* w, Capabilities caps);
  ~Cable();

  Wiring* wiring;
  Connector a;
  Connector b;
  Capabilities capabilities;
};

class Wiring : public Infrastructure {
  friend class Cable;
public:
  using Infrastructure::Infrastructure;

  void update() override;

private:
  std::vector<std::pair<Node*, Node*>> connections;
  std::vector<Cable> cables;

  Socket* findOther(Socket* socket);
};
