#include <Foundation/Systems/Text.hpp>

using namespace Text;

bool Text::System::filter(const Wiring::Connection& edge) const {
  return edge.capabilities.text.enabled;
}

void Text::System::swap(Wiring::Connection& edge) {
  std::swap(buffers[edge.a], buffers[edge.b]);
}

void Text::System::send(Wiring::Port* port, const std::string& m) {
  buffers[port].messages.push(m);
}

std::optional<std::string> Text::System::receive(Wiring::Port* port) {
  if (buffers[port].messages.empty()) {
    return std::nullopt;
  }
  else {
    std::string res = buffers[port].messages.front();
    buffers[port].messages.pop();
    return res;
  }
}
