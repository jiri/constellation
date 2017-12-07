#include <Foundation/Systems/Text.hpp>

bool TextSystem::filter(const Connection& edge) const {
  return edge.capabilities.text.enabled;
}

void TextSystem::swap(Connection& edge) {
  std::swap(buffers[edge.a], buffers[edge.b]);
}

void TextSystem::send(Port* port, const std::string& m) {
  buffers[port].messages.push(m);
}

std::optional<std::string> TextSystem::receive(Port* port) {
  if (buffers[port].messages.empty()) {
    return std::nullopt;
  }
  else {
    std::string res = buffers[port].messages.front();
    buffers[port].messages.pop();
    return res;
  }
}
