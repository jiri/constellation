#pragma once

#include <set>

#include <Foundation/Infrastructures/Infrastructure.hpp>

class Wiring;

class Connector {
public:
  virtual ~Connector() = default;

  virtual void merge(Connector* other) = 0;
  virtual void split(Connector* other) = 0;
  virtual glm::vec2& position() = 0;

  bool magnetic = false;
  Connector* other = nullptr;
};

class CableConnector : public Connector {
public:
  CableConnector(glm::vec2 pos)
    : cablePosition { new glm::vec2 { pos } }
  { }

  virtual ~CableConnector() {
    if (this->owner) {
      delete this->cablePosition;
    }
  }

  void merge(Connector* other) override;
  void split(Connector* other) override;

  glm::vec2& position() override {
    return *cablePosition;
  }

private:
  glm::vec2* cablePosition;
  bool owner = false;
};

class EndpointConnector : public Connector {
  friend class Wiring;
public:
  explicit EndpointConnector(Endpoint* e);

  void merge(Connector* other) override;
  void split(Connector* other) override;

  glm::vec2& position() override {
    fakePosition = endpoint->globalPosition();
    return fakePosition;
  }

private:
  Endpoint* endpoint;
  glm::vec2 fakePosition;
};

class Cable {
  friend class Wiring;
public:
  CableConnector a;
  CableConnector b;

private:
  Cable(glm::vec2 aPos, glm::vec2 bPos);
};

class Socket : public Endpoint {
  friend class Wiring;
public:
  EndpointConnector connector;

private:
  explicit Socket(Capabilities c);
};

class WiringConnection {
public:
  /* Ensures that a < b */
  WiringConnection(Connector* a, Connector* b, bool internal);

  Connector* a = nullptr;
  Connector* b = nullptr;
  bool internal = false;
};

static bool operator<(const WiringConnection& a, const WiringConnection& b) {
  if (a.a == b.a) {
    if (a.b == b.b) {
      return false;
    }
    else {
      return a.b < b.b;
    }
  }
  else {
    return a.a < b.a;
  }
}

class Wiring : public Infrastructure {
  friend void DrawGraph(Universe& universe);
public:
  using Infrastructure::Infrastructure;
  ~Wiring() final;

  void update() override;

  bool occupied(Connector* c) const;

  Socket* createSocket(Capabilities c);
  Cable* createCable(glm::vec2 aPos, glm::vec2 bPos);

private:
  std::set<WiringConnection> connections;
  std::vector<Cable*> cables;
  std::set<Connector*> connectors;

  void join(Connector* a, Connector* b, bool internal = false);
  void separate(Connector* a, Connector* b, bool internal = false);

  EndpointConnector* findOther(EndpointConnector* connector);
};
