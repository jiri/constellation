#pragma once

#include <unordered_map>

#include <Foundation/Systems/System.hpp>

namespace Energy {
  struct Buffer {
    float energyOffer = 0.0f;
    float energyPool = 0.0f;
  };

  struct System : public ::System {
    using ::System::System;

    bool filter(const Connection& edge) const override;
    void swap(Connection& edge) override;

    void offer(Wiring::Port* port, float energy);
    float request(Wiring::Port* port, float req);

    std::unordered_map<Wiring::Port*, Buffer> buffers;
  };
}
