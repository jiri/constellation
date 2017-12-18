#pragma once

#include <unordered_map>

#include <Foundation/Systems/System.hpp>

class EnergySystem : public System {
  using Buffer = float;
public:
  explicit EnergySystem(Universe* u)
    : System { u, "Energy" }
  { }

  bool filter(const Connection& edge) const override;
  void swap(Connection& edge) override;

  void update() override;

  void produce(Port* port, float energy);
  float consume(Port* port, float req);

  std::unordered_map<Port*, Buffer> sendBuffers;
  std::unordered_map<Port*, Buffer> recvBuffers;
};
