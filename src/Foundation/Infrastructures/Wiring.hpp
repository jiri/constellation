#pragma once

#include <optional>
#include <vector>

#include <Foundation/Infrastructures/Capabilities.hpp>

class Component;
struct Universe;

class Wiring {
public:
  struct Port;

  struct Node {
    explicit Node(Capabilities c)
      : capabilities(c)
    { }

    virtual ~Node() = default;

    virtual Port* findPort(Node* prev) = 0;
    virtual bool isFree() const = 0;
    virtual void addNeighbour(Node* node) = 0;
    virtual void removeNeighbour(Node* node) = 0;
    virtual bool isNeighbourOf(Node* node) const = 0;
    virtual std::optional<Capabilities> fold(Node* prev) = 0;

    Capabilities capabilities;
  };

  struct Port : public Node {
    Port(Capabilities cap);

    Port* findPort(Node*) override;
    bool isFree() const override;
    void addNeighbour(Node* node) override;
    void removeNeighbour(Node* node) override;
    bool isNeighbourOf(Node* node) const override;
    std::optional<Capabilities> fold(Node* prev) override;

    bool connected();
    std::string name() const;

    Component* component = nullptr;
    Node* neighbour = nullptr;
  };

  struct Cable : public Node {
    using Node::Node;

    Port* findPort(Node* prev) override;
    bool isFree() const override;
    void addNeighbour(Node *node) override;
    void removeNeighbour(Node* node) override;
    bool isNeighbourOf(Node* node) const override;
    std::optional<Capabilities> fold(Node *prev) override;

    Node* neighbours[2] {};
    uint8_t neighbourCount = 0;
  };

  static void connect(Universe& u, Node& a, Node& b);
  static void connect(Universe& u, Node* a, Node* b);

  static void disconnect(Universe& u, Node& a, Node& b);
  static void disconnect(Universe& u, Node* a, Node* b);
};
