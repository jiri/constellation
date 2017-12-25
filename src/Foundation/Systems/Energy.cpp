#include <Foundation/Systems/Energy.hpp>

#include <algorithm>
#include <queue>

#include <Foundation/Universe.hpp>

bool EnergySystem::filter(const Connection& edge) const {
  return edge.capabilities.energy.enabled;
}

void EnergySystem::swap(Connection& edge) {
}

void EnergySystem::produce(Port* port, float energy) {
  sendBuffers[port] += energy;
}

float EnergySystem::consume(Port* port, float req) {
  float res = std::min(recvBuffers[port], req);
  recvBuffers[port] -= res;
  return res;
}

void EnergySystem::update() {
  for (auto& pair : recvBuffers) {
    Port* p = pair.first;
    recvBuffers[p] = 0;
  }

  for (auto& pair : sendBuffers) {
    Port* p = pair.first;

    std::queue<std::pair<float, Port*>> queue;
    queue.emplace(sendBuffers[p], p);

    // TODO: This loops indefinitely when circuit is not complete
    while (!queue.empty()) {
      float e = queue.front().first;
      Port* p = queue.front().second;

      for (Connection& c : universe->connections) {
        if (c.from == p) {
          p = c.to;
          break;
        }
      }

      std::vector<std::pair<float, Port*>> neighbours = p->component->redistributeEnergy(p);

      if (neighbours.empty()) {
        recvBuffers[p] += e;
      }
      else {
        for (auto& pp : neighbours) {
          queue.emplace(pp.first * e, pp.second);
        }
      }

      queue.pop();
    }

    sendBuffers[p] = 0;
  }
}
