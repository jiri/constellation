#include <Foundation/Debugger.hpp>

#include <sstream>

#include <Foundation/Systems/Text.hpp>

void Debugger::addCommand(const std::string& name, std::variant<Command, VoidCommand, CommandVoid>&& v) {
  auto visitor = [this, &name](auto&& c) {
    using T = std::decay_t<decltype(c)>;
    if constexpr (std::is_same_v<T, Debugger::Command>) {
      commands[name] = c;
    }
    else if constexpr (std::is_same_v<T, Debugger::VoidCommand>) {
      commands[name] = [c](const std::vector<std::string>& ps) {
        c(ps);
        return "";
      };
    }
    else if constexpr (std::is_same_v<T, Debugger::CommandVoid>) {
      commands[name] = [c](const std::vector<std::string>& ps) {
        return c();
      };
    }
  };
  std::visit(visitor, v);
}

void Debugger::process() {
  while (auto s = component->universe->get<TextSystem>().receive(&component->port(port))) {
    std::istringstream iss(*s);

    std::vector<std::string> tokens {
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };

    try {
      if (!tokens.empty() && commands.count(tokens[0]) != 0) {
        auto res = commands[tokens[0]]({ ++tokens.begin(), tokens.end() });

        if (!res.empty()) {
          component->universe->get<TextSystem>().send(&component->port(port), res);
        }
      }
      else {
        component->universe->get<TextSystem>().send(&component->port(port), "Unknown command");
      }
    }
    catch (std::runtime_error& e) {
      component->universe->get<TextSystem>().send(&component->port(port), e.what());
    }
  }
}
