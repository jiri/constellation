#include <Foundation/Systems/Energy.hpp>

#include <algorithm>

bool EnergySystem::filter(const Connection& edge) const {
  return edge.capabilities.energy.enabled;
}

void EnergySystem::swap(Connection& edge) {
  auto t = edge.capabilities.energy.throughput;
  buffers[edge.from].energyPool = std::min(t, buffers[edge.to].energyOffer);
  buffers[edge.to].energyOffer = 0.0f;
  buffers[edge.to].energyPool = std::min(t, buffers[edge.from].energyOffer);
  buffers[edge.from].energyOffer = 0.0f;
}

void EnergySystem::offer(Port* port, float energy) {
  buffers[port].energyOffer += energy;
}

float EnergySystem::request(Port* port, float req) {
  float res = std::min(buffers[port].energyPool, req);
  buffers[port].energyPool -= res;
  return res;
}
