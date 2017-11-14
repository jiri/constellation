#pragma once

#include <boost/graph/adjacency_list.hpp>

#include <Foundation/Systems.hpp>

class Component;

class Wiring {
  Wiring() = default;

public:
  static Wiring& instance() {
    static Wiring w;
    return w;
  }

  struct Vertex {
    Component* component = nullptr;
  };

  struct Port;

  struct Edge {
    Port* a = nullptr;
    Port* b = nullptr;
    Capabilities capabilities;
  };

  struct Graph {
    using vertexHandle = size_t;

    vertexHandle addVertex(const Vertex& v) {
      vertices.push_back(v);
      return vertices.size() - 1;
    }

    std::vector<Vertex> vertices;
    std::vector<std::tuple<vertexHandle, vertexHandle, Edge>> edges;
  };

  Graph g;

  static Graph& graph() {
    return instance().g;
  }

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
    Port(Component* c, Capabilities cap);

    Port* findPort(Node*) override;
    bool isFree() const override;
    void addNeighbour(Node* node) override;
    void removeNeighbour(Node* node) override;
    bool isNeighbourOf(Node* node) const override;
    std::optional<Capabilities> fold(Node* prev) override;

    bool connected();

    Component* component = nullptr;
    Node* neighbour = nullptr;

    Picture::Buffer pictureBuffer;
    Energy::Buffer energyBuffer;
    Text::Buffer textBuffer;
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

  static void connect(Node& a, Node& b);
  static void connect(Node* a, Node* b);

  static void disconnect(Node& a, Node& b);
  static void disconnect(Node* a, Node* b);
};
