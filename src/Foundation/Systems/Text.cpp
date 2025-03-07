#include <Foundation/Systems/Text.hpp>

bool TextSystem::filter(const Connection& edge) const {
  return edge.capabilities.text.enabled;
}

void TextSystem::swap(Connection& edge) {
  std::swap(sendBuffers[edge.from], recvBuffers[edge.to]);
}

void TextSystem::send(Endpoint* port, const std::string& m) {
  sendBuffers[port].messages.push(m);
}

std::optional<std::string> TextSystem::receive(Endpoint* port) {
  if (recvBuffers[port].messages.empty()) {
    return std::nullopt;
  }
  else {
    std::string res = recvBuffers[port].messages.front();
    recvBuffers[port].messages.pop();
    return res;
  }
}
