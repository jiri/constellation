#pragma once

#include <boost/graph/adjacency_list.hpp>

#include <Foundation/Systems.hpp>

class Component;

namespace Wiring {
  struct Vertex {
    Component* component = nullptr;
  };

  struct vertex_property_t {
    typedef boost::edge_property_tag kind;
  };

  struct Port;

  struct Edge {
    Port* a = nullptr;
    Port* b = nullptr;
    Capabilities capabilities;
  };

  struct edge_property_t {
    typedef boost::edge_property_tag kind;
  };

  typedef boost::adjacency_list<boost::vecS,
                                boost::vecS,
                                boost::undirectedS,
                                boost::property<vertex_property_t, Vertex>,
                                boost::property<edge_property_t, Edge>> Graph;

  static Graph& graph() {
    static Graph g;
    return g;
  }

  typedef boost::property_map<Graph, edge_property_t>::type EdgePropertyMap;

  static const EdgePropertyMap& propertyMap() {
    return boost::get(edge_property_t(), Wiring::graph());
  };

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

  void connect(Node& a, Node& b);
  void connect(Node* a, Node* b);

  void disconnect(Node& a, Node& b);
  void disconnect(Node* a, Node* b);
}
