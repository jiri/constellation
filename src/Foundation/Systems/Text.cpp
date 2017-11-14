#include <Foundation/Systems/Text.hpp>

using namespace Text;

Capability Capability::combine(const Capability& a, const Capability& b) {
  return {
      a.enabled && b.enabled,
  };
}

void Buffer::send(const std::string& m) {
  messages.push(m);
}

std::optional<std::string> Buffer::receive() {
  if (messages.empty()) {
    return std::nullopt;
  }
  else {
    std::string res = messages.front();
    messages.pop();
    return res;
  }
}
