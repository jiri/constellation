#include <Foundation/Infrastructures/Wiring.hpp>

#include <algorithm>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <Foundation/Universe.hpp>
#include <Foundation/Components/Component.hpp>

Wiring::~Wiring() {
  for (auto* cable : this->cables) {
    delete cable;
  }
}

EndpointConnector* Wiring::findOther(EndpointConnector* connector) {
  Connector* prev = connector;
  Connector* next = nullptr;

  for (const WiringConnection& connection : this->connections) {
    if (connection.a == connector) {
      next = connection.b;
    }
    else if (connection.b == connector) {
      next = connection.a;
    }
  }

  bool stepMade = false;
  do {
    stepMade = false;
    for (auto& connection : connections) {
      if (connection.a == next || connection.b == next) {
        if (connection.a == prev || connection.b == prev) {
          continue;
        }
        else if (connection.a == next) {
          prev = next;
          next = connection.b;
          stepMade = true;
        }
        else if (connection.b == next) {
          prev = next;
          next = connection.a;
          stepMade = true;
        }
      }
    }
  } while (stepMade);

  return dynamic_cast<EndpointConnector*>(next);
}

void Wiring::update() {
  /* Update connections */
  for (auto* connector : this->connectors) {
    if (auto* from = dynamic_cast<EndpointConnector*>(connector)) {
      if (auto* to = this->findOther(from)) {
        this->connect(from->endpoint, to->endpoint, Capabilities{});
      }
    }
  }

  /* Disconnect broken connections */
  for (auto& connection : this->universe->connections) {
    if (connection.author == this) {
      auto* as = dynamic_cast<Socket*>(connection.from);
      auto* bs = dynamic_cast<Socket*>(connection.to);

      if (as && bs && (findOther(&as->connector) != &bs->connector || findOther(&bs->connector) != &as->connector)) {
        disconnect(as, bs);
      }
    }
  }

  /* Connect close connectors */
  for (Connector* a : this->connectors) {
    for (Connector* b : this->connectors) {
      if (a == b) {
        continue;
      }

      auto eitherMagnetic = a->magnetic || b->magnetic;
      auto bothFree = !this->occupied(a) && !this->occupied(b);
      auto bothClose = glm::distance(a->position(), b->position()) < 6.0f;

      if (eitherMagnetic && bothFree && bothClose) {
        this->join(a, b);
      }
    }
  }

  ImGui::Begin("Switchboard");

  if (ImGui::Button("Add Cable")) {
    cables.emplace_back(this->createCable(glm::vec2 { 0.0f, 0.0f }, glm::vec2 { 50.0f, 0.0f }));
  }

  ImGui::End();
}

bool Wiring::occupied(Connector* c) const {
  auto pred = [&](const WiringConnection& conn) {
    return (conn.a == c || conn.b == c) && !conn.internal;
  };

  return std::find_if(this->connections.begin(), this->connections.end(), pred) != this->connections.end();
}

void Wiring::join(Connector* a, Connector* b, bool internal) {
  WiringConnection connection { a, b, internal };

  if (this->connections.count(connection) != 0) {
    throw std::runtime_error { "Attempting to join already connected ports" };
  }

  this->connections.emplace(connection);

  if (!internal) {
    a->merge(b);
  }
}

void Wiring::separate(Connector* a, Connector* b, bool internal) {
  WiringConnection connection { a, b, internal };

  if (this->connections.count(connection) == 0) {
    throw std::runtime_error { "Attempting to separate unconnected ports" };
  }

  this->connections.erase(connection);

  if (!internal) {
    a->split(b);
  }
}

Socket* Wiring::createSocket(Capabilities c) {
  auto* ptr = new Socket { c };
  this->connectors.emplace(&ptr->connector);
  return ptr;
}

Cable* Wiring::createCable(glm::vec2 aPos, glm::vec2 bPos) {
  auto* ptr = new Cable { aPos, bPos };
  this->join(&ptr->a, &ptr->b, true);
  this->connectors.emplace(&ptr->a);
  this->connectors.emplace(&ptr->b);
  return ptr;
}

void CableConnector::merge(Connector* other) {
  if (auto* c = dynamic_cast<CableConnector*>(other)) {
    auto* newPosition = new glm::vec2 { 0.5f * (*this->cablePosition + *c->cablePosition) };

    delete this->cablePosition;
    this->cablePosition = newPosition;
    this->owner = true;

    delete c->cablePosition;
    c->cablePosition = newPosition;
    c->owner = false;
  }
  else if (auto* e = dynamic_cast<EndpointConnector*>(other)) {
    delete this->cablePosition;
    this->cablePosition = &e->position();
    this->owner = false;
  }

  this->other = other;
  other->other = this;
}

void CableConnector::split(Connector* other) {
  const glm::vec2 offset { 10.0f, 10.0f };

  if (auto* c = dynamic_cast<CableConnector*>(other)) {
    glm::vec2 oldPos = this->position();

    delete this->cablePosition;

    this->cablePosition = new glm::vec2 { oldPos + offset };
    this->owner = true;
    c->cablePosition = new glm::vec2 { oldPos - offset };
    c->owner = true;
  }
  else if (auto* e = dynamic_cast<EndpointConnector*>(other)) {
    this->cablePosition = new glm::vec2 { e->position() + offset };
    this->owner = true;
  }

  this->other = nullptr;
  other->other = nullptr;
}

void EndpointConnector::merge(Connector* other) {
  if (auto* c = dynamic_cast<CableConnector*>(other)) {
    c->merge(this);
  }
  else if (auto* e = dynamic_cast<EndpointConnector*>(other)) {
    throw std::runtime_error { "Attempting to merge two endpoints" };
  }
}

void EndpointConnector::split(Connector* other) {
  if (auto* c = dynamic_cast<CableConnector*>(other)) {
    c->split(this);
  }
  else if (auto* e = dynamic_cast<EndpointConnector*>(other)) {
    throw std::runtime_error { "Attempting to split two endpoints" };
  }
}

EndpointConnector::EndpointConnector(Endpoint* e)
  : endpoint { e }
{ }

Cable::Cable(glm::vec2 aPos, glm::vec2 bPos)
  : a { aPos }
  , b { bPos }
{ }

WiringConnection::WiringConnection(Connector* a, Connector* b, bool internal)
  : a { a }
  , b { b }
  , internal { internal }
{
  if (this->a > this->b) {
    std::swap(this->a, this->b);
  }
}

Socket::Socket(Capabilities c)
  : Endpoint { c }
  , connector { this }
{ }
