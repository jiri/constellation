#pragma once

#include <unordered_map>

#include <Foundation/Systems/System.hpp>

class EnergySystem : public System {
public:
  struct Buffer {
    float energyOffer = 0.0f;
    float energyPool = 0.0f;
  };

  using System::System;

  bool filter(const Connection& edge) const override;
  void swap(Connection& edge) override;

  void offer(Port* port, float energy);
  float request(Port* port, float req);

  std::unordered_map<Port*, Buffer> buffers;
};
