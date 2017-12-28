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

  void produce(Endpoint* port, float energy);
  float consume(Endpoint* port, float req);

  std::unordered_map<Endpoint*, Buffer> sendBuffers;
  std::unordered_map<Endpoint*, Buffer> recvBuffers;
};
