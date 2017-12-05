#include <Foundation/Systems/Energy.hpp>

#include <algorithm>

bool Energy::System::filter(const Connection& edge) const {
  return edge.capabilities.energy.enabled;
}

void Energy::System::swap(Connection& edge) {
  auto t = edge.capabilities.energy.throughput;
  buffers[edge.a].energyPool = std::min(t, buffers[edge.b].energyOffer);
  buffers[edge.b].energyOffer = 0.0f;
  buffers[edge.b].energyPool = std::min(t, buffers[edge.a].energyOffer);
  buffers[edge.a].energyOffer = 0.0f;
}

void Energy::System::offer(Wiring::Port* port, float energy) {
  buffers[port].energyOffer += energy;
}

float Energy::System::request(Wiring::Port* port, float req) {
  float res = std::min(buffers[port].energyPool, req);
  buffers[port].energyPool -= res;
  return res;
}
