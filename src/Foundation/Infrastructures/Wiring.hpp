#pragma once

#include <list>

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Wiring;

class Node {
public:
  virtual ~Node() = default;
};

class Socket : public Endpoint, public Node {
public:
  using Endpoint::Endpoint;
};

class Cable;

class Connector : public Node {
public:
  Connector(Cable* c, glm::vec2 pos)
    : cable { c }
    , position { pos }
  { }

  Cable* cable;
  glm::vec2 position;
};

class Cable {
public:
  Cable(Wiring* w, glm::vec2 aPos, glm::vec2 bPos, Capabilities caps);
  ~Cable();

  Wiring* wiring;
  Connector a;
  Connector b;
  Capabilities capabilities;
};

class Wiring : public Infrastructure {
  friend class Cable;
  friend void DrawGraph(Universe& universe);
public:
  using Infrastructure::Infrastructure;
  virtual ~Wiring();

  void update() override;

private:
  std::vector<std::pair<Node*, Node*>> connections;
  std::vector<Cable*> cables;

  Socket* findOther(Socket* socket);

  void join(Node* a, Node* b);
  void separate(Node* a, Node* b);
};
